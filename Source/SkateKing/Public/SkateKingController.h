// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "SkateKingController.generated.h"

class ASkateboardCharacter;

UCLASS()
class SKATEKING_API ASkateKingController : public APlayerController
{
	GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    // Input actions
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    class UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    class UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    class UInputAction* SpeedUpAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    class UInputAction* SlowDownAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    class UInputMappingContext* DefaultMappingContext;

private:
    void Move(const FInputActionValue& Value);
    void SpeedUp(const FInputActionValue& Value);
    void SlowDown(const FInputActionValue& Value);
    void Jump(const FInputActionValue& Value);

    ASkateboardCharacter* SkateboardCharacter;
	
};
