// Fill out your copyright notice in the Description page of Project Settings.


#include "SkateboardCharacter.h"
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

    MovementInput = FVector2D::ZeroVector;
    bIsSpeedingUp = false;
    bIsSlowingDown = false;

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
        // Apply acceleration in the direction of input
        CurrentVelocity += DesiredMovementDirection * Acceleration * DeltaTime;
        //UE_LOG(LogTemp, Log, TEXT("Applying Acceleration: CurrentVelocity = %s, MovementInput = %s"), *CurrentVelocity.ToString(), *MovementInput.ToString());
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

