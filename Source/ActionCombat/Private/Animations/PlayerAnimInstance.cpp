// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/PlayerAnimInstance.h"

// Updates the player's movement speed for animations
void UPlayerAnimInstance::UpdateSpeed()
{
	// Get the owned pawn
	APawn* PawnRef{ TryGetPawnOwner() };

	// Stop if the pawn is not valid
	if (!IsValid(PawnRef)) { return; }
	// Get velocity and calculate current speed
	FVector Velocity{ PawnRef->GetVelocity() };
	
	CurrentSpeed = static_cast<float>(Velocity.Length());
	
}
// Called when the lock-on target is changes
void UPlayerAnimInstance::HandleUpdatedTarget(AActor* NewTargetActorRef)
{
	// Set combat state based on whether a target is locked-on
	bIsInCombat = IsValid(NewTargetActorRef);
}

// Updates the direction the player is moving in for animation blending
void UPlayerAnimInstance::UpdatedDirection()
{
	// Get the owned pawn 
	APawn* PawnRef{ TryGetPawnOwner() };

	// Only update direction if pawn is valid and in combat
	if (!IsValid(PawnRef)) { return; }
	if (!bIsInCombat) { return; }

	// Calculate movement direction relative to the player's facing direction
	CurrentDirection = CalculateDirection(
		PawnRef->GetVelocity(),
		PawnRef->GetActorRotation()
	);
}
