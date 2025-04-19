// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/LockOnComponent.h"
#include "Gameframework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Interfaces/Enemy.h"

/**
 * Lock-on component for targeting enemies in combat.
 * Manages detecting, locking onto, and tracking enemies, along with combat state updates.
 * broadcasting target changes for use in UI or animation.
 */

// Sets default values for this component's properties
ULockOnComponent::ULockOnComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();

	// Stores references to essential components
	OwnerRef = GetOwner<ACharacter>();
	Controller = GetWorld()->GetFirstPlayerController();
	MovementComp = OwnerRef->GetCharacterMovement();
	SpringArmComp = OwnerRef->FindComponentByClass< USpringArmComponent >();
}

// Called every frame
void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//	If there's no valid target, skip
	if (!IsValid(CurrentTargetActor)) { return; }

	//	Get positions of owner and target
	FVector CurrentLocation = { OwnerRef->GetActorLocation()};
	FVector TargetLocation = { CurrentTargetActor->GetActorLocation()};
	
	//	Calculate distance to target
	double TargetDistance {
		FVector::Distance(CurrentLocation, TargetLocation)
	};
	
	// End lock-on if target is too far
	if (TargetDistance >= BreakDistance)
	{
		EndLockOn();
		return;
	}
	
	// Slightly adjust the target's location to focus lower
	TargetLocation.Z -= 125;

	// Calculate rotation needed to face the target
	FRotator NewRotation{UKismetMathLibrary::FindLookAtRotation(
	CurrentLocation, TargetLocation	
	) };

	// Apply the new rotation to the controller
	Controller->SetControlRotation(NewRotation);
}

// Starts locking on to the closest valid target within a radius
void ULockOnComponent::StartLockOn(float Radius)
{
	FHitResult OutResult;
	FVector CurrentLocation { OwnerRef->GetActorLocation() };
	FCollisionShape Sphere{ FCollisionShape::MakeSphere(Radius) };
	FCollisionQueryParams IgnoreParams{
		FName{ TEXT("Ignore Collision Params") },
		false,
		OwnerRef
	};

	// Perform sphere trace to find enemy
	bool bHasFoundTarget{ GetWorld()->SweepSingleByChannel(
		OutResult,
		CurrentLocation,
		CurrentLocation,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		Sphere,
		IgnoreParams
	) };

	// If nothing was hit, return
	if (!bHasFoundTarget) { return; }

	// Make sure the hit actor implements the enemy interface
	if (!OutResult.GetActor()->Implements<UEnemy>()) { return; }

	// Stores the actor as the current target
	CurrentTargetActor = OutResult.GetActor();

	// Disable player camera control and configure movement for lock-on
	// so character rotation is driven by the controller (auto-aimed at the target)
	Controller->SetIgnoreLookInput(true);
	MovementComp->bOrientRotationToMovement = false;
	MovementComp->bUseControllerDesiredRotation = true;

	// Adjust the camera boom for better framing
	SpringArmComp->TargetOffset = FVector{ 0.0, 0.0, 100.0 };

	// Tell the enemy it's been selected for lock-on (To show UI)
	IEnemy::Execute_OnSelect(CurrentTargetActor);

	// Broadcast event to notify other systems that the target was updated
	OnUpdatedTargetDelegate.Broadcast(CurrentTargetActor);
	
	// Logs the current Actor selected for debugging to see if lock on works
	// UE_LOG(
	// 	LogTemp, Warning, TEXT("Actor Detected: %s"),
	// 	*OutResult.GetActor()->GetName()
	// 	);
	
}
// Ends the lock-on and resets orientation settings
void ULockOnComponent::EndLockOn()
{
	// Tell the enemy it's been deselected from lock-on (To disable UI)
	IEnemy::Execute_OnDeselect(CurrentTargetActor);
	
	// Clear current target
	CurrentTargetActor = nullptr;
	
	// Restore movement orientation settings
	MovementComp->bOrientRotationToMovement = true;
	MovementComp->bUseControllerDesiredRotation = false;
	SpringArmComp->TargetOffset = FVector::ZeroVector;

	// Re-enable normal look input
	Controller->ResetIgnoreLookInput();

	// Broadcast event to notify systems that there is no longer a target
	OnUpdatedTargetDelegate.Broadcast(CurrentTargetActor);
}

// Toggle between starting or ending lock-on based on current state
void ULockOnComponent::ToggleLockOn(float Radius)
{
	if (IsValid(CurrentTargetActor))
	{
		EndLockOn();
	}
	else
	{
		StartLockOn(Radius);
	}
}


