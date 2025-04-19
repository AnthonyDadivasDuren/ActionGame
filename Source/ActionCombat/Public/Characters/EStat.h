// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "EStat.generated.h"

UENUM(BlueprintType)
enum class EStat : uint8 
{
	None UMETA(DisplayName = "None Selected"),
	Health UMETA(DisplayName = "Vitality"),
	MaxHealth UMETA(DisplayName = "Max Vitality"),
	Strength UMETA(DisplayName = "Might"),
	Stamina UMETA(DisplayName = "Endurance"),
	MaxStamina UMETA(DisplayName = "Max Endurance")
};