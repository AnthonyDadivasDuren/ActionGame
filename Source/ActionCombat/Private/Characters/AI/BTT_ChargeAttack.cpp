// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AI/BTT_ChargeAttack.h"
#include "AIController.h"
#include "Animations/BossAnimInstance.h"
#include "GameFramework/Character.h"
#include  "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/EEnemyState.h"
#include "Navigation/PathFollowingComponent.h"


// Monitors the blackboard component to check if the AI is ready to perform a charge attack.
// When ready, it initiates the charge attack sequence and resets the charge flag.
void UBTT_ChargeAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	bool bIsReadyToCharge{ OwnerComp.GetBlackboardComponent()
		->GetValueAsBool(TEXT("IsReadyToCharge"))
		
	};

	// Reset charge flag and start charging
	if (bIsReadyToCharge)
	{
		OwnerComp.GetBlackboardComponent()
			->SetValueAsBool(
				TEXT("IsReadyToCharge"), false
			);
		ChargeAtPlayer();
	}

	

	if (!bIsFinished) { return; }

	OwnerComp.GetBlackboardComponent()->SetValueAsEnum(
		TEXT("CurrentState"), EEnemyState::Melee
	);

	ControllerRef->ReceiveMoveCompleted.Remove(MoveCompleteDelegate);
	
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	
}


// Constructor that enables tick functionality for continuous monitoring
// and sets up the delegate to handle movement completion events
UBTT_ChargeAttack::UBTT_ChargeAttack()
{
	bNotifyTick = true;

	MoveCompleteDelegate.BindUFunction(
		this, "HandleMoveCompleted"
	);

}

// Initializes the charge attack by setting up necessary references and animation state
// Returns InProgress to keep the behavior tree task running during the charge
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

	bIsFinished = false;
	
	// Return InProgress since charging is an ongoing action
	return EBTNodeResult::InProgress;
}

// Configures and executes the AI movement towards the player
// Sets up pathfinding parameters, movement request, and focuses the AI on the player
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

	ControllerRef->ReceiveMoveCompleted.AddUnique(MoveCompleteDelegate);

	OriginalWalkSpeed = CharacterRef->GetCharacterMovement()
		->MaxWalkSpeed;

	CharacterRef->GetCharacterMovement()->MaxWalkSpeed = ChargeWalkSpeed;
	
}

// Called when the AI reaches its destination or stops charging
// Resets the charging animation state and starts a 1-second cooldown timer
void UBTT_ChargeAttack::HandleMoveCompleted()
{
	BossAnim->bIsCharging = false;

	FTimerHandle AttackTimerHandle;
	
	CharacterRef->GetWorldTimerManager().SetTimer(
		AttackTimerHandle,
		this,
		&UBTT_ChargeAttack::FinishAttackTask,
		PostChargePauseTime,
		false
	);

	CharacterRef->GetCharacterMovement()
		->MaxWalkSpeed = OriginalWalkSpeed;
	
}

// Finalizes the charge attack sequence after the cooldown period
// Logs completion message and allows the behavior tree to continue
void UBTT_ChargeAttack::FinishAttackTask()
{
	//UE_LOG(LogTemp, Warning, TEXT("Task Finished!"))

	bIsFinished = true;

	

	
}