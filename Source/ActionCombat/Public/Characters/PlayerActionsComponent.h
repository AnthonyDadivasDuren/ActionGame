// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerActionsComponent.generated.h"

/*
 *	Handles player movement states and actions (sprint, walk, etc.)
 *
 */


// Delegate for sprint events, broadcasts stamina cost

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(
	FOnSprintSignature,
	UPlayerActionsComponent, OnSprintDelegate,
	float, Cost
);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONCOMBAT_API UPlayerActionsComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	// Reference to the owned character
	ACharacter* CharacterRef;

	// Interface reference for player functionality
	class IMainPlayer* IPlayerRef;

	// Reference to movement component for speed control
	class UCharacterMovementComponent* MovementComp;

	// Stamina cost for sprint
	UPROPERTY(EditAnywhere)
	float SprintCost { 0.1f };

	// Character movement speed while sprinting
	UPROPERTY(EditAnywhere)
	float SprintSpeed { 1000.0f };

	// Character movement speed while walking
	UPROPERTY(EditAnywhere)
	float WalkSpeed { 500.0f };

public:	
	// Sets default values for this component's properties
	UPlayerActionsComponent();

	// Delegate for sprint events
	UPROPERTY(BlueprintAssignable)
	FOnSprintSignature OnSprintDelegate;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Initiates sprint if enough stamina is available
	UFUNCTION(BlueprintCallable)
	void Sprint();

	// Returns to walking speed
	UFUNCTION(BlueprintCallable)
	void Walk();
		
};
