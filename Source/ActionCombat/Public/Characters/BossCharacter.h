// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/Enemy.h"
#include "Characters/EEnemyState.h"
#include "Interfaces/Fighter.h"
#include "BossCharacter.generated.h"

UCLASS()
class ACTIONCOMBAT_API ABossCharacter : public ACharacter, public IEnemy, public IFighter
{
	GENERATED_BODY()

private:
	
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EEnemyState> InitialState;

	class UBlackboardComponent* BlackboardComp;

	UPROPERTY(EditAnywhere)
	UAnimMontage* DeathAnim;

	class AAIController* ControllerRef;


	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* RearAttackMontage;
    
	UPROPERTY(EditAnywhere, Category = "Combat")
	float BehindCheckTime = 1.0f; // Time player needs to stay behind before triggering rear attack
    
	UPROPERTY(EditAnywhere, Category = "Combat")
	float BehindAngleThreshold = 120.0f; // Angle that defines "behind" the boss (in degrees)
    
	float TimeBehindBoss = 0.0f;
	bool bIsPlayerBehind = false;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	float TurnSpeed = 8.0f;
    
	bool bIsTurning = false;
	FRotator TargetRotation;

	bool bIsStunned = false;

    
	FTimerHandle RearAttackTimerHandle;
	FTimerHandle StunTimerHandle;
	UPROPERTY(EditAnywhere)

	UAnimMontage* StunAnimMontage;


public:
	// Sets default values for this character's properties
	ABossCharacter();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UStatsComponent* StatsComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCombatComponent* CombatComp;




protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void DetectPawn(APawn* DetectedPawn, APawn* PawnToDetect);

	virtual float GetDamage() override;

	virtual void Attack() override;
	
	virtual float GetAnimDuration() override;

	virtual float GetMeleeRange() override;

	UFUNCTION()
	void HandlePlayerDeath();

	UFUNCTION(BlueprintCallable)
	void HandleDeath();

	UFUNCTION()
	void FinishDeathAnim();
	
	UFUNCTION(BlueprintCallable)
	void StunCharacter(float Duration);

	virtual bool IsBlocking() const override;
	virtual bool IsParrying() const override;

	void CheckPlayerPosition();
	void PerformRearAttack();
	bool IsPlayerBehind() const;
	
};