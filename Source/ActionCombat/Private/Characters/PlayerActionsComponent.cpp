// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerActionsComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/MainPlayer.h"
#include "Kismet/KismetMathLibrary.h"


/*
 *	Implementation of player movement states and actions
 */


// Sets default values for this component's properties
UPlayerActionsComponent::UPlayerActionsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlayerActionsComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get references to required components and interfaces
	CharacterRef = GetOwner<ACharacter>();
	MovementComp = CharacterRef->GetCharacterMovement();

	// Ensure owner implements the player interface
	if (!CharacterRef->Implements<UMainPlayer>()) { return; }

	IPlayerRef = Cast<IMainPlayer>(CharacterRef);
	
}


// Called every frame
void UPlayerActionsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPlayerActionsComponent::Sprint()
{
      /*
      if (IPlayerRef == nullptr)
        {
            UE_LOG(LogTemp, Warning, TEXT("IPlayerRef is invalid!"));
            return;
        }

	// Log the stamina check result
	bool bHasEnoughStamina = IPlayerRef->HasEnoughStamina(SprintCost);
	UE_LOG(LogTemp, Warning, TEXT("Stamina check result: %d"), bHasEnoughStamina);

	if (!bHasEnoughStamina) { return; }

	if (MovementComp == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("MovementComp is invalid!"));
		return;
	}
	*/
	

	// Check if player has enough stamina to sprint
	if (!IPlayerRef->HasEnoughStamina(SprintCost))
	{
		// If not enough stamina, return to walking
		Walk();
		return;
	}

	// Don't sprint if character isn't moving
	if (MovementComp->Velocity.Equals(FVector::ZeroVector, 1)) { return; }

	// Set character movement speed to sprint speed
	MovementComp->MaxWalkSpeed = SprintSpeed;
	// Broadcast sprint event with stamina cost
	OnSprintDelegate.Broadcast(SprintCost);
}

void UPlayerActionsComponent::Walk()
{
	// Set character movement speed to walk speed
	MovementComp->MaxWalkSpeed = WalkSpeed;
}

void UPlayerActionsComponent::Roll()
{
	if (bIsRollActive || !IPlayerRef->HasEnoughStamina(RollCost)) { return; }

	bIsRollActive = true;

	OnRollDelegate.Broadcast(RollCost);

	FVector Direction{
		CharacterRef->GetCharacterMovement()->Velocity.Length() < 1 ?
			CharacterRef->GetActorForwardVector() :
			CharacterRef->GetLastMovementInputVector()
	};

	FRotator NewRot{ UKismetMathLibrary::MakeRotFromX(Direction) };

	CharacterRef->SetActorRotation(NewRot);
	float Duration { CharacterRef->PlayAnimMontage(RollAnimMontage) };
	FTimerHandle RollTimerHandle;

	CharacterRef->GetWorldTimerManager().SetTimer(
		RollTimerHandle,
		this,
		&UPlayerActionsComponent::FinishRollAnim,
		Duration,
		false
		);
	
}

void UPlayerActionsComponent::FinishRollAnim()
{
	bIsRollActive = false;
}

