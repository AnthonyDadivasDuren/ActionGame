// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyProjectileComponent.generated.h"

/**
 * Component responsible for handling projectile spawning for enemy actors
 * Can be attached to any enemy that needs to fire projectiles
 * Provides blueprint-callable functionality to spawn projectiles from specified spawn points
 */


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONCOMBAT_API UEnemyProjectileComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyProjectileComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SpawnProjectile(
		FName ComponentName, TSubclassOf<AActor> ProjectileClass
		);
		
};
