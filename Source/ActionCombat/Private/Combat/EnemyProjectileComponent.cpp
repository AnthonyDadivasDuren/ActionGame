// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/EnemyProjectileComponent.h"

/**
 * 
 *	Component that handles projectile spawning functionality for enemy actors
 */



// Sets default values for this component's properties
UEnemyProjectileComponent::UEnemyProjectileComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyProjectileComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UEnemyProjectileComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UEnemyProjectileComponent::SpawnProjectile(FName ComponentName, TSubclassOf<AActor> ProjectileClass)
{
	// Get the spawn point component from the owner actor
	USceneComponent* SpawnPointComp{
		Cast<USceneComponent>(GetOwner()->GetDefaultSubobjectByName(ComponentName)
			)
		};

	// Get the world location where we want to spawn the projectile
	FVector SpawnLocation{ SpawnPointComp->GetComponentLocation() };

	// Spawn the projectile at the specified location
	GetWorld()->SpawnActor(ProjectileClass, &SpawnLocation);
}

