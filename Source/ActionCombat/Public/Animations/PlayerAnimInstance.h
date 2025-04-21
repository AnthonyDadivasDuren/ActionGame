// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * Animation instance class for the player character.
 * Handles movement speed, direction, and combat state for animation blending.
 */
UCLASS()
class ACTIONCOMBAT_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	// The current movement speed of the player (used in blendspaces)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentSpeed{ 0.0f };

	// Updates the CurrentSpeed based on player velocity
	UFUNCTION(BlueprintCallable)
	void UpdateSpeed();

	// True if the player is currently in combat (e.g., has a lock-on target)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsInCombat{ false };

	// Direction of movement relative to character's faced Direction
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentDirection{ 0.0f };
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsBlocking{ false };
	
	// Updates combat state based on lock-on target presence
	UFUNCTION(BlueprintCallable)
	void HandleUpdatedTarget(AActor* NewTargetActorRef);
	// Updates movement direction for use in directional blends
	UFUNCTION(BlueprintCallable)
	void UpdatedDirection();
	
};
