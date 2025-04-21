// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AI/BTT_ChargeAttack.h"
#include "AIController.h"
#include "Animations/BossAnimInstance.h"
#include "GameFramework/Character.h"
#include  "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"


/**
 * Executes the charge attack behavior tree task
 * InProgress to indicate the charge attack is ongoing
 */


void UBTT_ChargeAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	bool bIsReadyToCharge{ OwnerComp.GetBlackboardComponent()
		->GetValueAsBool(TEXT("IsReadyToCharge"))

	};

	if (bIsReadyToCharge)
	{
		OwnerComp.GetBlackboardComponent()
			->SetValueAsBool(
				TEXT("IsReadyToCharge"), false
			);

		ChargeAtPlayer();
	}
}

UBTT_ChargeAttack::UBTT_ChargeAttack()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_ChargeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Get the AI controller reference from the behavior
	ControllerRef = OwnerComp.GetAIOwner();

	// Get the character being controlled by the AI
	CharacterRef = ControllerRef->GetCharacter();

	// Get and cast the animation instance to our custom boss animation instance
	BossAnim = Cast<UBossAnimInstance>(
		CharacterRef->GetMesh()->GetAnimInstance()
		);

	// Trigger the charging state in the animation blueprint
	BossAnim->bIsCharging = true;

	// Reset the charge readiness flag in the blackboard
	// This prevents multiple charge attempts until the condition is met again
	OwnerComp.GetBlackboardComponent()
		->SetValueAsBool(
			TEXT("IsReadyToCharge"), false
		);

	
	// Return InProgress since charging is an ongoing action
	return EBTNodeResult::InProgress;
}

void UBTT_ChargeAttack::ChargeAtPlayer()
{
	//UE_LOG(LogTemp, Warning, TEXT("Running"))

	APawn* PlayerRef{
		GetWorld()->GetFirstPlayerController()->GetPawn()
	};
	FVector PlayerLocation{ PlayerRef->GetActorLocation() };

	FAIMoveRequest MoveRequest{	PlayerLocation };
	MoveRequest.SetUsePathfinding(true);
	MoveRequest.SetAcceptanceRadius(AcceptableRadius);

	ControllerRef->MoveTo(MoveRequest);
	ControllerRef->SetFocus(PlayerRef);
	
}
