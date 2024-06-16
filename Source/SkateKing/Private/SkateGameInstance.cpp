// Fill out your copyright notice in the Description page of Project Settings.


#include "SkateGameInstance.h"

USkateGameInstance::USkateGameInstance()
    : CurrentPoints(0)
{
}

void USkateGameInstance::AddPoints(int32 Points)
{
    CurrentPoints += Points;
}

int32 USkateGameInstance::GetPoints() const
{
    return CurrentPoints;
}

