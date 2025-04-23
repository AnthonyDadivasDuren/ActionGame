// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/BlockComponent.h"
#include "GameFramework/Character.h"
#include "interfaces/MainPlayer.h"
#include "Characters/BossCharacter.h"

// Sets default values for this component's properties
UBlockComponent::UBlockComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBlockComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBlockComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UBlockComponent::Check(AActor* Opponent)
{
	ACharacter* CharacterRef{ GetOwner<ACharacter>() };

	if (!CharacterRef->Implements<UMainPlayer>()) { return true; }

	IMainPlayer* PlayerRef{ Cast<IMainPlayer>(CharacterRef) };

	FVector OpponentForward{ Opponent->GetActorForwardVector() };
	FVector PlayerForward{ CharacterRef->GetActorForwardVector() };

	double Result{FVector::DotProduct(OpponentForward, PlayerForward) };

	if (Result > 0 || !PlayerRef->HasEnoughStamina(StaminaCost))
	{
		return true;
	}

	CharacterRef->PlayAnimMontage(BlockAnimMontage);

	OnBlockDelegate.Broadcast(StaminaCost);
	
	
	return false;
}


bool UBlockComponent::AttemptParry(AActor* Attacker)
{
	if (!bCanParry) { return false; }

	// Start parry window
	bCanParry = false;
	GetWorld()->GetTimerManager().SetTimer(
		ParryWindowTimerHandle,
		this,
		&UBlockComponent::OnParryWindowEnd,
		ParryWindow,
		false
	);

	return true;
}

void UBlockComponent::OnParryWindowEnd()
{
	bCanParry = true;
}

void UBlockComponent::OnSuccessfulParry(AActor* ParriedActor)
{
	// Cast to boss character
	ABossCharacter* Boss = Cast<ABossCharacter>(ParriedActor);
	if (!Boss) return;

	// Stun the boss
	Boss->StunCharacter(ParryStunDuration);
}



