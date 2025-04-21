// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/MainPlayer.h"
#include "Interfaces/Fighter.h"
#include "MainCharacter.generated.h"

/*
 * Main playable character class that implements:
 * - Basic character functionality (movement, collision, etc.)
 * - Custom player actions (sprint, walk)
 * - Combat capabilities (damage, blocking, targeting)
 */

UCLASS()
class ACTIONCOMBAT_API AMainCharacter : public ACharacter, public IMainPlayer, public IFighter
{
	GENERATED_BODY()

private:
	
	UPROPERTY(EditAnywhere)
	UAnimMontage* DeathAnimMontage;

public:
	// Sets default values for this character's properties
	AMainCharacter();

	// Component for managing character stats like health and stamina
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UStatsComponent* StatsComp;
	
	// Component for handling target locking in combat
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class ULockOnComponent* LockOnComp;
	
	// Component for managing combat actions and states
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UCombatComponent* CombatComp;
	
	// Component for handling various trace operations
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UTraceComponent* TraceComp;
	
	// Component for managing blocking mechanics
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UBlockComponent* BlockComp;
	
	// Component for handling player movement states and actions
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UPlayerActionsComponent* PlayerActionsComp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Reference to the character's animation instance
	UPROPERTY(BlueprintReadOnly)
	class UPlayerAnimInstance* PlayerAnim;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Returns the character's current damage value (from strength stat)
	virtual float GetDamage() override;

	// Checks if character has enough stamina for an action
	virtual bool HasEnoughStamina(float Cost) override;

	UFUNCTION(BlueprintCallable)
	void HandleDeath();
};