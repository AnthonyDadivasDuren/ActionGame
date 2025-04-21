// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ChargeAttack.generated.h"

// Behavior Tree Task that handles the boss's charge attack behavior
// Controls movement, animation states, and attack timing
UCLASS()
class ACTIONCOMBAT_API UBTT_ChargeAttack : public UBTTaskNode
{
    GENERATED_BODY()
    
private:
    // Controls the AI character's movement and behavior during the charge attack
    AAIController* ControllerRef;

    // The actual boss character that performs the charge attack
    ACharacter* CharacterRef;

    // Manages the boss's animation states and transitions during charging
    class UBossAnimInstance* BossAnim;

    // Minimum distance required between the boss and its target to consider the charge complete
    UPROPERTY(EditAnywhere)
    float AcceptableRadius{ 250.0f };

    // Handles the event when the charge movement is complete or interrupted
    FScriptDelegate MoveCompleteDelegate;

    float OriginalWalkSpeed;

    UPROPERTY(EditAnywhere)
    float ChargeWalkSpeed { 2000.0f };

    bool bIsFinished { false }; 

protected:
    // Updates the charge attack state each frame, checking if conditions are met to begin charging
    virtual void TickTask(
        UBehaviorTreeComponent& OwnerComp,
        uint8* NodeMemory,
        float DeltaSeconds)
        override;
    
public:
    // Sets up initial task parameters and binds the movement completion delegate
    UBTT_ChargeAttack();
    
    // Initializes the charge attack sequence and sets up necessary references
    virtual EBTNodeResult::Type ExecuteTask(
        UBehaviorTreeComponent& OwnerComp,
        uint8* NodeMemory
            ) override;

    // Initiates the movement towards the player with pathfinding
    void ChargeAtPlayer();

    // Processes the completion of the charge movement
    UFUNCTION()
    void HandleMoveCompleted();

    // Completes the charge attack sequence after cooldown
    UFUNCTION()
    void FinishAttackTask();
};