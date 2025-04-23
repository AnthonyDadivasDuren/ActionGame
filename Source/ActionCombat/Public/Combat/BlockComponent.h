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
	float ParryStunDuration { 2.0f }; // How long the boss stays stunned after parry

	UPROPERTY(EditAnywhere)
	float BlockDamageReduction { 0.5f }; // Reduces damage by 50% when blocking

	UPROPERTY(EditAnywhere)
	float ParryStartupWindow = 0.1f;  // Time before parry becomes active

	float BlockStartTime  { 0.0f };

	
	UPROPERTY(EditAnywhere)
	UAnimMontage* BlockAnimMontage;

	bool bCanParry{ true };
	bool bInParryWindow{ false };
	bool bIsBlocking { false };

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

	// Returns true if block was successful
	UFUNCTION(BlueprintCallable)
	bool Check(AActor* Opponent);

	// Returns the reduced damage amount when blocking
	float GetReducedDamage(float IncomingDamage) const;
	
	// Returns true if parry was successful
	UFUNCTION(BlueprintCallable)
	bool AttemptParry(AActor* Attacker);

	// Called when successfully parrying an attack
	void OnSuccessfulParry(AActor* ParriedActor);

	UFUNCTION(BlueprintCallable)
	void StartBlocking();

	UFUNCTION(BlueprintCallable)
	void StopBlocking();

	
private:
	
	UFUNCTION()
	void OnParryWindowEnd();
		
};
