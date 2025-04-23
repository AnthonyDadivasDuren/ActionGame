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
	bBlockFailed = false;  // Reset at start of check

	
	if (!bIsBlocking) return true;
    
	ACharacter* CharacterRef{ GetOwner<ACharacter>() };
	if (!CharacterRef->Implements<UMainPlayer>()) { return true; }
    
	IMainPlayer* PlayerRef{ Cast<IMainPlayer>(CharacterRef) };
	if (!PlayerRef->HasEnoughStamina(StaminaCost))
	{
		bBlockFailed = true;  // Failed due to stamina
		return true;
	}

	FVector OpponentForward{ Opponent->GetActorForwardVector() };
	FVector PlayerForward{ CharacterRef->GetActorForwardVector() };
	double Result{ FVector::DotProduct(OpponentForward, PlayerForward) };

	// Allow blocking in a wider angle (less than 0.5 means roughly 120 degree arc)
	if (Result > 0.5f)
	{
		bBlockFailed = true;  // Failed due to wrong angle
		return true;
	}  

	// Play block animation and consume stamina
	CharacterRef->PlayAnimMontage(BlockAnimMontage);
	OnBlockDelegate.Broadcast(StaminaCost);

	// Since block was successful, make sure bIsBlocking is true
	// This ensures the TraceComponent will show the block effect
	bIsBlocking = true;
    
	return false;

}




bool UBlockComponent::AttemptParry(AActor* Attacker)
{
	if (!bIsBlocking || !bCanParry || !bInParryWindow) return false;
    
	float CurrentTime = GetWorld()->GetTimeSeconds();
	float BlockDuration = CurrentTime - BlockStartTime;
    
	// Only allow parry within the initial window
	if (BlockDuration > ParryWindow)
	{
		return false;
	}
	bIsParrying = true;
	bCanParry = false;
	return true;



}

void UBlockComponent::OnParryWindowEnd()
{
	bInParryWindow = false;
	bIsParrying = false;
	bCanParry = true;


}

void UBlockComponent::OnSuccessfulParry(AActor* ParriedActor)
{
	// Cast to boss character
	ABossCharacter* Boss = Cast<ABossCharacter>(ParriedActor);
	if (!Boss) return;

	Boss->StunCharacter(ParryStunDuration);

}



float UBlockComponent::GetReducedDamage(float IncomingDamage) const
{
	return IncomingDamage * BlockDamageReduction;  


}

void UBlockComponent::StartBlocking()
{
	if (!bIsBlocking)
	{
		BlockStartTime = GetWorld()->GetTimeSeconds();
		bCanParry = true;
		bInParryWindow = true;
		bIsBlocking = true;
        
		// Set timer to end parry window
		GetWorld()->GetTimerManager().SetTimer(
			ParryWindowTimerHandle,
			this,
			&UBlockComponent::OnParryWindowEnd,
			ParryWindow,
			false
		);
	}

}

void UBlockComponent::StopBlocking()
{
	bIsBlocking = false;
	bInParryWindow = false;
	bCanParry = true;
}



