// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SkateGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SKATEKING_API USkateGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
    USkateGameInstance();

    // Function to add points
    UFUNCTION(BlueprintCallable, Category = "Points")
    void AddPoints(int32 Points);

    // Function to get the current points
    UFUNCTION(BlueprintCallable, Category = "Points")
    int32 GetPoints() const;

private:
    // Variable to store points
    int32 CurrentPoints;
	
};
