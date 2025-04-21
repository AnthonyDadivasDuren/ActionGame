// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AI/BTT_RangeAttack.h"
#include "AIController.h"
#include  "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/EEnemyState.h"

/*
* Executes the ranged attack behavior tree task
*/

EBTNodeResult::Type UBTT_RangeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//UE_LOG(LogTemp, Warning, TEXT("Sucess!"))

	
	// Get the AI character
	ACharacter* CharacterRef{
		OwnerComp.GetAIOwner()->GetPawn<ACharacter>()
	};

	// Fail if character is invalid
	if (!IsValid(CharacterRef)) { return EBTNodeResult::Failed; }

	
	
	// Play the attack animation montage

	CharacterRef->PlayAnimMontage(AnimMontage);

	double RandomValue { UKismetMathLibrary::RandomFloat() };

	if (RandomValue > Threshold)
	{
		Threshold = 0.9;
		//UE_LOG(LogTemp, Warning, TEXT("Charging at the Player!"))

		OwnerComp.GetBlackboardComponent()
			->SetValueAsEnum(
				TEXT("CurrentState"),
				EEnemyState::Charge
				);
	}
	else
	{
		Threshold -= 0.1;
	}
	
	return EBTNodeResult::Succeeded;
	
	
}
