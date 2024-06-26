// Fill out your copyright notice in the Description page of Project Settings.


#include "SkateboardCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "SkateGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/MovementComponent.h"

// Sets default values
ASkateboardCharacter::ASkateboardCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Set default values
    Acceleration = 500.0f;
    Deceleration = 300.0f;
    MaxSpeed = 1200.0f;
    MaxBackSpeed = 300.0f;
    JumpImpulse = 500.0f;
    Friction = 0.5f;  // Default friction value
    SpeedUpMultiplier = 2.0f;

    MovementInput = FVector2D::ZeroVector;
    bIsSpeedingUp = false;
    bIsSlowingDown = false;

    // Set turn rates for input
    BaseTurnRate = 45.f;
    BaseLookUpRate = 45.f;

    RotationAmount = 45.0f;
    TiltMultiplier = 15.0f;

    //Obstacle Detection
    bIsInAir = false;
    bHasCheckedObstacles = false;
    bHasReachedMaxHeight = false;

    SphereTraceRadius = 50.0f;

    // Create CameraBoom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character
    CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

    // Create FollowCamera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

// Called when the game starts or when spawned
void ASkateboardCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASkateboardCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    ApplyMovement(DeltaTime);


    if (GetCharacterMovement()->IsFalling())
    {
        if (!bIsInAir)
        {
            bIsInAir = true;
            bHasCheckedObstacles = false;
            bHasReachedMaxHeight = false;
        }

        if (!bHasReachedMaxHeight && GetCharacterMovement()->Velocity.Z <= 0)
        {
            bHasReachedMaxHeight = true;
        }

        if (bHasReachedMaxHeight && !bHasCheckedObstacles)
        {
            CheckForObstacles();
        }
    }
    else
    {
        bIsInAir = false;
        bHasReachedMaxHeight = false;
    }

}


void ASkateboardCharacter::ApplyMovement(float DeltaTime)
{
    // Get the mesh's forward and right vectors with an adjustment for the -90 degree yaw rotation
    FRotator MeshRotation = GetMesh()->GetComponentRotation();
    MeshRotation.Yaw += 90.0f; // Adjust for the -90 degree yaw rotation

    FVector ForwardDirection = FRotationMatrix(MeshRotation).GetUnitAxis(EAxis::X);
    FVector RightDirection = FRotationMatrix(MeshRotation).GetUnitAxis(EAxis::Y);
    FVector DesiredMovementDirection = (ForwardDirection * MovementInput.Y + RightDirection * MovementInput.X).GetSafeNormal();

    if (!MovementInput.IsNearlyZero())
    {
        // Determine the actual acceleration based on whether the player is speeding up
        float ActualAcceleration = bIsSpeedingUp ? Acceleration * SpeedUpMultiplier : Acceleration;

        // Apply acceleration in the direction of input
        CurrentVelocity += DesiredMovementDirection * ActualAcceleration * DeltaTime;

        // Apply tilt based on horizontal movement input (X-axis)
        float TiltAmount = FMath::Clamp(MovementInput.X, -1.0f, 1.0f);
        float TargetTilt = TiltAmount * TiltMultiplier;  // Adjust the tilt amount as needed; negative sign to correct direction

        // Get current tilt and interpolate towards the target tilt
        FRotator CurrentRotation = GetMesh()->GetRelativeRotation();
        FRotator TargetRotation = FRotator(TargetTilt, CurrentRotation.Yaw, CurrentRotation.Roll);  // Correct the tilt to be left to right
        FRotator SmoothRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 5.0f);  // Adjust the interpolation speed as needed

        GetMesh()->SetRelativeRotation(SmoothRotation);

        // Apply rotation based on horizontal movement input (X-axis)
        float RotationInput = MovementInput.X * RotationAmount * DeltaTime;
        AddActorLocalRotation(FRotator(0.0f, RotationInput, 0.0f));
    }
    else
    {
        // Reset tilt smoothly
        FRotator CurrentRotation = GetMesh()->GetRelativeRotation();
        FRotator TargetRotation = FRotator(0.0f, CurrentRotation.Yaw, CurrentRotation.Roll);
        FRotator SmoothRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 5.0f);  // Adjust the interpolation speed as needed
        GetMesh()->SetRelativeRotation(SmoothRotation);
    }

    if (bIsSlowingDown)
    {
        // Apply deceleration when the player is slowing down
        CurrentVelocity -= CurrentVelocity.GetSafeNormal() * Deceleration * DeltaTime;
    }
    else
    {
        // Apply friction when there is no input
        ApplyFriction(DeltaTime);
    }

    // Clamp velocity to MaxSpeed for forward movement and MaxBackSpeed for backward movement
    float MaxAllowedSpeed = (CurrentVelocity | ForwardDirection) >= 0 ? MaxSpeed : MaxBackSpeed;

    if (CurrentVelocity.Size() > MaxAllowedSpeed)
    {
        CurrentVelocity = CurrentVelocity.GetSafeNormal() * MaxAllowedSpeed;
    }

    if (CurrentVelocity.SizeSquared() < KINDA_SMALL_NUMBER)
    {
        CurrentVelocity = FVector::ZeroVector;
    }

    FVector MovementDelta = CurrentVelocity * DeltaTime;

    // Perform a safe move to handle collisions smoothly
    FHitResult HitResult;
    GetCharacterMovement()->SafeMoveUpdatedComponent(MovementDelta, GetActorRotation().Quaternion(), true, HitResult);

    // If we hit something, stop the movement
    if (HitResult.IsValidBlockingHit())
    {
        CurrentVelocity = FVector::ZeroVector;
    }
}

void ASkateboardCharacter::ApplyFriction(float DeltaTime)
{
    if (GetCharacterMovement()->IsMovingOnGround() && CurrentVelocity.Size() > 0)
    {
        FVector FrictionForce = -CurrentVelocity.GetSafeNormal() * Friction * DeltaTime;
        if (FrictionForce.Size() > CurrentVelocity.Size())
        {
            CurrentVelocity = FVector::ZeroVector;
        }
        else
        {
            CurrentVelocity += FrictionForce;
        }
    }
}



void ASkateboardCharacter::CheckForObstacles()
{
    FVector Start = GetActorLocation() + CurrentVelocity * 0.1f; // Adjust start based on velocity
    FVector End = Start - FVector(0, 0, 200.0f); // Adjust the ray length as needed

    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this);

    bool bHit = GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(SphereTraceRadius), CollisionParams);

    if (bHit)
    {
        float ObstacleHeight = Start.Z - HitResult.ImpactPoint.Z;
        UE_LOG(LogTemp, Log, TEXT("Obstacle detected. Height: %f"), ObstacleHeight);
        AwardPoints(ObstacleHeight);
        bHasCheckedObstacles = true;

        // Draw debug line and point
        DrawDebugLine(GetWorld(), Start, HitResult.ImpactPoint, FColor::Green, false, 1.0f, 0, 5.0f);
        DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10.0f, FColor::Red, false, 1.0f);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("No obstacle detected."));

        // Draw debug line
        DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 5.0f);
    }
}

void ASkateboardCharacter::AwardPoints(float ObstacleHeight)
{
    if (USkateGameInstance* SkateGameInstance = Cast<USkateGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
        int32 Points = 0;

        // Modular system for awarding more points for smaller obstacles
        if (ObstacleHeight <= 50.0f) // Adjust thresholds as needed
        {
            Points = 100;
        }
        else if (ObstacleHeight <= 100.0f)
        {
            Points = 50;
        }
        else if (ObstacleHeight <= 200.0f)
        {
            Points = 25;
        }

        UE_LOG(LogTemp, Log, TEXT("Awarded %d points for obstacle height: %f"), Points, ObstacleHeight);
        SkateGameInstance->AddPoints(Points);

        // Play sound at the player's location
        if (PointsAwardedSound && Points != 0)
        {
            UGameplayStatics::PlaySoundAtLocation(this, PointsAwardedSound, GetActorLocation());
        }
    }
}
