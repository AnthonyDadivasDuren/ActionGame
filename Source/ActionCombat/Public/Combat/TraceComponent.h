// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/FTraceSockets.h"
#include "TraceComponent.generated.h"


UENUM(BlueprintType)
enum class EHitEffectType : uint8
{
	Normal  UMETA(DisplayName = "Normal Hit"),
	Block   UMETA(DisplayName = "Block"),
	Parry   UMETA(DisplayName = "Parry")
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONCOMBAT_API UTraceComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	// Reference to the character's skeletal mesh (used for socket positions)
	USkeletalMeshComponent* SkeletalComp;

	UPROPERTY(EditAnywhere)
	TArray<FTraceSockets> Sockets;
	
	// Length of the box collision (width/depth is constant)
	UPROPERTY(EditAnywhere)
	double BoxCollisionLength { 30.0 };

	// Toggle For DebugMode (Draws Debug box for the trace)
	UPROPERTY(EditAnywhere)
	bool bDebugMode { false };

	// List of actors already hit during this attack to avoid duplicates
	TArray<AActor*> TargetsToIgnore;

	UPROPERTY(EditAnywhere)
	UParticleSystem* HitParticleTemplate;

	UPROPERTY(EditAnywhere)
	UParticleSystem* BlockParticleTemplate;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ParryParticleTemplate;


	// Helper function to spawn appropriate hit effect
	void SpawnHitEffect(const FVector& Location, EHitEffectType HitType);

	
public:	
	// Sets default values for this component's properties
	UTraceComponent();

	// Whether the character is currently attacking (triggers trace in Tick)
	UPROPERTY(VisibleAnywhere)
	bool bIsAttacking { false };

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Clears the list of already hit targets (called when attack ends)
	UFUNCTION(BlueprintCallable)
	void HandleResetAttack();

	
			
};
