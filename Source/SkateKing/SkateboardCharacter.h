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

	// Character movement properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Acceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Deceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float JumpImpulse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Friction;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FVector CurrentVelocity;
	FVector2D MovementInput;
	bool bIsSpeedingUp;
	bool bIsSlowingDown;

	void ApplyMovement(float DeltaTime);
	void ApplyFriction(float DeltaTime);
	friend class ASkateKingController;

};
