// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SkateboardCharacter.generated.h"

UCLASS()
class SKATEKING_API ASkateboardCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASkateboardCharacter();

	// Camera boom positioning the camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
	class USpringArmComponent* CameraBoom;

	// Follow camera
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
	class UCameraComponent* FollowCamera;

	// Base turn rate, in deg/sec. Other scaling may affect final turn rate.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate;

	// Base look up/down rate, in deg/sec. Other scaling may affect final rate.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpRate;

	// Character movement properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Acceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Deceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SpeedUpMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxBackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float JumpImpulse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Friction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RotationAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float TiltMultiplier;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector2D MovementInput;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FVector CurrentVelocity;
	bool bIsSpeedingUp;
	bool bIsSlowingDown;

	void ApplyMovement(float DeltaTime);
	void ApplyFriction(float DeltaTime);

	// Raycast to check for obstacles during jump
	void CheckForObstacles();

	// Award points based on obstacle height
	void AwardPoints(float ObstacleHeight);

	// Flags to track jumping state
	bool bIsInAir;
	bool bHasCheckedObstacles;

	friend class ASkateKingController;

};
