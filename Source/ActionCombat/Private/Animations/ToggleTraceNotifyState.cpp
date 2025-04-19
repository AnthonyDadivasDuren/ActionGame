// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/ToggleTraceNotifyState.h"
#include "Combat/TraceComponent.h"

void UToggleTraceNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	// Enable attack trace when the animation starts
	SetTraceComponentState(MeshComp, true);
}

void UToggleTraceNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	// Disable attack trace when the animation ends
	SetTraceComponentState(MeshComp, false);
}

void UToggleTraceNotifyState::SetTraceComponentState(USkeletalMeshComponent* MeshComp, bool bIsAttacking)
{
	if (!MeshComp) return; // Safety check: Mesh is valid

	// Get the owner of the mesh (usually the character)
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	// Try to find the trace component on the owner
	UTraceComponent* TraceComponent = Owner->FindComponentByClass<UTraceComponent>();
	if (!IsValid(TraceComponent)) return;

	// Set the attacking state
	TraceComponent->bIsAttacking = bIsAttacking;
}
