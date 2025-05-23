// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/CombatComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Interfaces/MainPlayer.h"


// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}



// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterRef = GetOwner<ACharacter>();
	
}



// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCombatComponent::ComboAttack()
{
	if (CharacterRef->Implements<UMainPlayer>())
	{
		IMainPlayer* IPlayerRef{ Cast<IMainPlayer>(CharacterRef) };
		// If succesfully cast & if has enough stamina
		if (IPlayerRef && !IPlayerRef->HasEnoughStamina(StaminaCost))
		{
			return;
		}
	}
	
	if (!bCanAttack) { return; }

	// Clear any existing reset timer when attacking
	GetWorld()->GetTimerManager().ClearTimer(ComboResetTimerHandle);

	bCanAttack = false;
	
	CharacterRef->PlayAnimMontage(AttackAnimations[ComboCounter]);
	
	ComboCounter++;

	int MaxCombo{ AttackAnimations.Num() };

	ComboCounter = UKismetMathLibrary::Wrap(
		ComboCounter,
		-1,
		(MaxCombo - 1)
		);
	
	OnAttackPerformedDelegate.Broadcast(StaminaCost);
	
}

void UCombatComponent::HandleResetAttack()
{
	bCanAttack = true;

	// Add debug message to verify the function is being called
	//UE_LOG(LogTemp, Warning, TEXT("HandleResetAttack called"));

	
	// Start the combo reset timer when the attack animation ends
	GetWorld()->GetTimerManager().SetTimer(
		ComboResetTimerHandle,
		this,
		&UCombatComponent::ResetCombo,
		ComboResetTime,
		false
	);

}

void UCombatComponent::RandomAttack()
{
	if (AttackAnimations.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No attack animations assigned to CombatComponent"));
		return;
	}

	
	int RandomIndex{
		FMath::RandRange(0, AttackAnimations.Num() - 1)
	};

	if (!AttackAnimations[RandomIndex])
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid animation montage at index %d"), RandomIndex);
		return;
	}


	AnimDuration = CharacterRef
		->PlayAnimMontage(AttackAnimations[RandomIndex]);
}

void UCombatComponent::ResetCombo()
{
	//UE_LOG(LogTemp, Warning, TEXT("ResetCombo called - Counter was: %d"), ComboCounter);

	ComboCounter = 0;
	bCanAttack = true;
}



