// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LockOnComponent.generated.h"

/**
 * Lock-on component for targeting enemies in combat.
 */

// Delegate for broadcasting when the lock-on target changes
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(
	FOnUpdatedTargetSignature,
	ULockOnComponent, OnUpdatedTargetDelegate,
	AActor*, NewTargetActorRef
);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONCOMBAT_API ULockOnComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	// Reference to the owned character
	ACharacter* OwnerRef;
	// Reference to the player controlle
	APlayerController* Controller;
	// Reference to the character's movement component
	class UCharacterMovementComponent* MovementComp;
	// Reference to the character's spring arm (camera boom)
	class USpringArmComponent* SpringArmComp;
	
public:	
	// Sets default values for this component's properties
	ULockOnComponent();

	// Currently locked-on target actor
	AActor* CurrentTargetActor;

	// Event that gets triggered when the target is updated (bind in Blueprint or code)
	UPROPERTY(BlueprintAssignable)
	FOnUpdatedTargetSignature OnUpdatedTargetDelegate;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Initiates lock-on to a nearby target
	UFUNCTION(BlueprintCallable)
	void StartLockOn(float Radius = 750.0f);

	// Ends the current lock-on
	void EndLockOn();

	// Toggles between starting and ending lock-on
	UFUNCTION(BlueprintCallable)
	void ToggleLockOn(float Radius = 750.0f);

	//  lock-on max distance which is automatically broken when exceeded
	UPROPERTY(EditAnywhere)
	double BreakDistance { 1000.0 };

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};

