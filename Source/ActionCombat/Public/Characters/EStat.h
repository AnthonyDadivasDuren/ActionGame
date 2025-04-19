// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "EStat.generated.h"

/*
 *	Enumeration of character statistics and attributes
 */

// Defines all available character statistics
UENUM(BlueprintType)
enum class EStat : uint8 
{
	None UMETA(DisplayName = "None Selected"),    // Default/invalid state
	Health UMETA(DisplayName = "Vitality"),       // Current health value
	MaxHealth UMETA(DisplayName = "Max Vitality"), // Maximum health capacity
	Strength UMETA(DisplayName = "Might"),        // Character's attack power
	Stamina UMETA(DisplayName = "Endurance"),     // Current stamina value
	MaxStamina UMETA(DisplayName = "Max Endurance") // Maximum stamina capacity

};