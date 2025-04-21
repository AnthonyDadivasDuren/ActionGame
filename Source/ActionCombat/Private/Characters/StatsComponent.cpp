// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/StatsComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/EStat.h"

/*
 *	- Manages character statistics and resources (health, stamina, etc.)
 *  - Handles resource regeneration and reduction
 *  - Controls stamina regeneration timing
 */

// Sets default values for this component's properties
UStatsComponent::UStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UStatsComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


/*
 * Reduces character's health by specified amount
 * Will not reduce health if character is already at 0 health
 * Ensures health stays within valid range (0 to MaxHealth)
 */
void UStatsComponent::ReduceHealth(float Amount)
{
	// Don't process damage if already dead
	if (Stats[EStat::Health] <= 0) { return; }

	Stats[EStat::Health] -= Amount;

	//Stops Health from going below 0
	Stats[EStat::Health] = UKismetMathLibrary::FClamp( 
		Stats[EStat::Health],
		0,
		Stats[EStat::MaxHealth]
	);

	OnHealthPercentUpdateDelegate.Broadcast(
		GetStatPercentage(EStat::Health, EStat::MaxHealth)
		);

	
}

/*
 * Reduces character's stamina and starts regeneration delay
 * Clamps stamina between 0 and MaxStamina
 * Sets up delayed regeneration using RetriggerableDelay
 */
void UStatsComponent::ReduceStamina(float Amount)
{
	// Reduce and clamp stamina value
	Stats[EStat::Stamina] -= Amount;
	Stats[EStat::Stamina] = UKismetMathLibrary::FClamp( 
		Stats[EStat::Stamina],
		0,
		Stats[EStat::MaxStamina]
	);
	
	// Disable regeneration temporarily
	bCanRegen = false;

	// Setup delayed regeneration
	FLatentActionInfo FunctionInfo{
		0,
		100,
		TEXT("EnableRegen"),
		this
	};
	
	// Start regeneration delay timer
	UKismetSystemLibrary::RetriggerableDelay(
		GetWorld(),
		StaminaDelayDuration,
		FunctionInfo
	);

	OnStaminaPercentUpdateDelegate.Broadcast(
		GetStatPercentage(EStat::Stamina, EStat::MaxStamina)
		);
	
}

/*
 * Handles stamina regeneration over time
 * Only regenerates if regeneration is enabled
 * Smoothly interpolates towards maximum stamina
 */
void UStatsComponent::RegenStamina()
{
	// Check if regeneration is allowed
	if (!bCanRegen) { return; }
	
	// Smoothly regenerate stamina
	Stats[EStat::Stamina] = UKismetMathLibrary::FInterpTo_Constant(
		Stats[EStat::Stamina],
		Stats[EStat::MaxStamina],
		GetWorld()->DeltaTimeSeconds,
		StaminaRegenRate
	);
	OnStaminaPercentUpdateDelegate.Broadcast(GetStatPercentage(
		EStat::Stamina, EStat::MaxStamina)
		);
}

/*
 * Enables stamina regeneration
 * Called after regeneration delay timer completes
 */
void UStatsComponent::EnableRegen()
{
	bCanRegen = true;
}

float UStatsComponent::GetStatPercentage(EStat Current, EStat Max)
{
	return (Stats[Current] / Stats[Max]);
}
