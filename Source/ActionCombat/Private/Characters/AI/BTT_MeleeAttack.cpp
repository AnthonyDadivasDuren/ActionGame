// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/AI/BTT_MeleeAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Interfaces/Fighter.h"
#include "GameFramework/Character.h"
#include "Characters/EEnemyState.h"
#include "Navigation/PathFollowingComponent.h"

// Implementation of the ExecuteTask function for melee attack behavior
EBTNodeResult::Type UBTT_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // Reset completion flag at start of execution
    bIsFinished = false;
    
    // Get current distance to target from blackboard
    float Distance {
        OwnerComp.GetBlackboardComponent()
        ->GetValueAsFloat(TEXT("Distance"))
    };
    
    // Get AI controller reference
    AAIController* AIRef{ OwnerComp.GetAIOwner() };
    
    // If target is outside attack range, move towards them
    if (Distance > AttackRadius)
    {
        // Get player pawn as target
        APawn* PlayerRef { GetWorld()->GetFirstPlayerController()->GetPawn()};

        // Configure movement request
        FAIMoveRequest MoveRequest{ PlayerRef };
        MoveRequest.SetUsePathfinding(true);
        MoveRequest.SetAcceptanceRadius(AcceptableRadius);

        // Setup movement completion callback
        AIRef->ReceiveMoveCompleted.AddUnique(MoveDelegate);
        
        // Start movement and focus on target
        AIRef->MoveTo(MoveRequest);
        AIRef->SetFocus(PlayerRef);
    }
    else // Target is in attack range
    {
        // Get Fighter interface from AI character
        IFighter* FighterRef{
            Cast<IFighter>(AIRef->GetCharacter())
        };

        // Execute attack
        FighterRef->Attack();

        // Set timer to complete task after attack animation
        FTimerHandle AttackTimerHandle;
        AIRef->GetCharacter()->GetWorldTimerManager().SetTimer(
            AttackTimerHandle,
            this,
            &UBTT_MeleeAttack::FinishAttackTask,
            FighterRef->GetAnimDuration(),
            false
        );
    }
    
    // Task needs time to complete, return InProgress
    return EBTNodeResult::InProgress;
}

// Tick function to monitor task status
void UBTT_MeleeAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    // Get current distance to target
    float Distance {
        OwnerComp.GetBlackboardComponent()->GetValueAsFloat(TEXT("Distance"))
    };

    AAIController* AIRef{ OwnerComp.GetAIOwner() };
    IFighter* FighterRef{
        Cast<IFighter>(AIRef->GetCharacter())
    };

    // Check if target moved out of melee range
    if (Distance > FighterRef->GetMeleeRange())
    {
        // Switch to range state
        OwnerComp.GetBlackboardComponent()
            ->SetValueAsEnum(TEXT("CurrentState"), EEnemyState::Range);
        
        // Cleanup and abort task
        AbortTask(OwnerComp, NodeMemory);
        FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
        AIRef->StopMovement();
        AIRef->ClearFocus(EAIFocusPriority::Gameplay);
        AIRef->ReceiveMoveCompleted.Remove(MoveDelegate);
    }
    
    // Continue ticking if task isn't finished
    if (!bIsFinished){ return; }

    // Clean up and complete task
    AIRef->ReceiveMoveCompleted.Remove(MoveDelegate);
    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

// Constructor
UBTT_MeleeAttack::UBTT_MeleeAttack()
{
    // Enable if multiple enemies are needed in the scene
    //bCreateNodeInstance = true;
    
    // Bind the completion callback
    MoveDelegate.BindUFunction(this, "FinishAttackTask");

    // Enable tick updates for this task
    bNotifyTick = true;
}

// Callback for task completion
void UBTT_MeleeAttack::FinishAttackTask()
{
    // Mark task as complete
    bIsFinished = true;
}