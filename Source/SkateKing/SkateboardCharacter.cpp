// Fill out your copyright notice in the Description page of Project Settings.


#include "SkateboardCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ASkateboardCharacter::ASkateboardCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Set default values
    Acceleration = 500.0f;
    Deceleration = 300.0f;
    MaxSpeed = 1200.0f;
    JumpImpulse = 500.0f;
    Friction = 0.5f;  // Default friction value
    SpeedUpMultiplier = 2.0f;

    MovementInput = FVector2D::ZeroVector;
    bIsSpeedingUp = false;
    bIsSlowingDown = false;

    // Set turn rates for input
    BaseTurnRate = 45.f;
    BaseLookUpRate = 45.f;

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

}

void ASkateboardCharacter::ApplyMovement(float DeltaTime)
{
    FVector ForwardDirection = GetActorForwardVector();
    FVector RightDirection = GetActorRightVector();
    FVector DesiredMovementDirection = (ForwardDirection * MovementInput.Y + RightDirection * MovementInput.X).GetSafeNormal();

    if (!MovementInput.IsNearlyZero())
    {
        // Determine the actual acceleration based on whether the player is speeding up
        float ActualAcceleration = bIsSpeedingUp ? Acceleration * SpeedUpMultiplier : Acceleration;

        // Apply acceleration in the direction of input
        CurrentVelocity += DesiredMovementDirection * ActualAcceleration * DeltaTime;

        UE_LOG(LogTemp, Log, TEXT("Applying Acceleration: CurrentVelocity = %s, MovementInput = %s"), *CurrentVelocity.ToString(), *MovementInput.ToString());
    }
    else if (bIsSlowingDown)
    {
        // Apply deceleration when the player is slowing down
        CurrentVelocity -= CurrentVelocity.GetSafeNormal() * Deceleration * DeltaTime;
    }
    else
    {
        // Apply friction when there is no input
        ApplyFriction(DeltaTime);
    }

    // Clamp velocity to MaxSpeed
    if (CurrentVelocity.Size() > MaxSpeed)
    {
        CurrentVelocity = CurrentVelocity.GetSafeNormal() * MaxSpeed;
    }

    if (CurrentVelocity.SizeSquared() < KINDA_SMALL_NUMBER)
    {
        CurrentVelocity = FVector::ZeroVector;
    }

    FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
    SetActorLocation(NewLocation);

    //UE_LOG(LogTemp, Log, TEXT("End of ApplyMovement: CurrentVelocity = %s, MovementInput = %s"), *CurrentVelocity.ToString(), *MovementInput.ToString());
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
        UE_LOG(LogTemp, Log, TEXT("Applying Friction: CurrentVelocity = %s, FrictionForce = %s"), *CurrentVelocity.ToString(), *FrictionForce.ToString());
    }
}
