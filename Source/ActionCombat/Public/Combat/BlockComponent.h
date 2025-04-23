// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlockComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(
	FOnBlockSignature,
	UBlockComponent, OnBlockDelegate,
	float, Cost
);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONCOMBAT_API UBlockComponent : public UActorComponent
{
	GENERATED_BODY()
	
private:
	
	UPROPERTY(EditAnywhere)
	float StaminaCost{ 10.0f };

	UPROPERTY(EditAnywhere)
	float ParryWindow = 0.2f; // Time window in seconds for successful parry

	UPROPERTY(EditAnywhere)
	float ParryStunDuration = 2.0f; // How long the boss stays stunned after parry
	
	UPROPERTY(EditAnywhere)
	UAnimMontage* BlockAnimMontage;

	bool bCanParry{ true };

	FTimerHandle ParryWindowTimerHandle;
public:	
	// Sets default values for this component's properties
	UBlockComponent();

	UPROPERTY(BlueprintAssignable)
	FOnBlockSignature OnBlockDelegate;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	bool Check(AActor* Opponent);


	void OnParryWindowEnd();

	// Returns true if parry was successful
	bool AttemptParry(AActor* Attacker);

	// Called when successfully parrying an attack
	void OnSuccessfulParry(AActor* ParriedActor);
		
};
