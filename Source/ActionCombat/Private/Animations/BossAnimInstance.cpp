// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/BossAnimInstance.h"

void UBossAnimInstance::UpdateSpeed()
{
	// Get the owned pawn
	APawn* PawnRef{ TryGetPawnOwner() };

	// Stop if the pawn is not valid
	if (!IsValid(PawnRef)) { return; }
	// Get velocity and calculate current speed
	FVector Velocity{ PawnRef->GetVelocity() };
	
	CurrentSpeed = static_cast<float>(Velocity.Length());
}
