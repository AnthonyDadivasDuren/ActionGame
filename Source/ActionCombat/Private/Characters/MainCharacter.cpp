// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MainCharacter.h"
#include "Animations/PlayerAnimInstance.h"
#include "Characters/StatsComponent.h"
#include "Characters/EStat.h"
#include "Combat/LockOnComponent.h"
#include "Combat/CombatComponent.h"
#include "Combat/TraceComponent.h"
#include "Combat/BlockComponent.h"
#include "Characters/PlayerActionsComponent.h"

/*
 *	 Implementation of the main playable character
 *
 */


// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create and initialize all character components
	StatsComp = CreateDefaultSubobject<UStatsComponent>(TEXT("Stats Component"));
	LockOnComp = CreateDefaultSubobject<ULockOnComponent>(TEXT("LockOn Component"));
	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
	TraceComp = CreateDefaultSubobject<UTraceComponent>(TEXT("Trace Component"));
	BlockComp = CreateDefaultSubobject<UBlockComponent>(TEXT("Block Component"));
	PlayerActionsComp = CreateDefaultSubobject<UPlayerActionsComponent>(TEXT("Player Actions Component"));
	
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Get reference to the character's animation instance
	PlayerAnim = Cast<UPlayerAnimInstance>(
		GetMesh()->GetAnimInstance()
		);
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AMainCharacter::GetDamage()
{
	// Return character's strength stat as damage value
	return StatsComp->Stats[EStat::Strength];
}

bool AMainCharacter::HasEnoughStamina(float Cost)
{
	// Check if current stamina is sufficient for the requested action
	return StatsComp->Stats[EStat::Stamina] >= Cost;

}

void AMainCharacter::HandleDeath()
{
	PlayAnimMontage(DeathAnimMontage);

	DisableInput(GetController<APlayerController>());
}

void AMainCharacter::EndLockOnWithActor(AActor* ActorRef)
{
	if (LockOnComp->CurrentTargetActor != ActorRef) { return; }

	LockOnComp->EndLockOn();
}

bool AMainCharacter::CanTakeDamage(AActor* Opponent)
{
	if (PlayerAnim->bIsBlocking)
	{
		return BlockComp->Check(Opponent);
	}
	
	return true;
}

