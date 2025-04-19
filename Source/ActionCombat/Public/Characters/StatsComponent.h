
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Characters/EStat.h"
#include "StatsComponent.generated.h"

/*
 *  - Manages character statistics and resources (health, stamina, etc.)
 *
 */

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONCOMBAT_API UStatsComponent : public UActorComponent
{
    GENERATED_BODY()

    // Rate at which stamina regenerates (units per second)
    UPROPERTY(EditAnywhere)
    double StaminaRegenRate{ 10.0 };

    // Flag indicating if stamina regeneration is currently allowed
    UPROPERTY(VisibleAnywhere)
    bool bCanRegen{ true };

    // Time to wait after stamina use before regeneration begins
    UPROPERTY(EditAnywhere)
    float StaminaDelayDuration{ 2.0f };

public:    
    // Constructor - sets default component properties
    UStatsComponent();

    // Map containing all character statistics and their current values
    UPROPERTY(EditAnywhere)
    TMap<EStat, float> Stats;
    
protected:
    // Initialization when game starts
    virtual void BeginPlay() override;

public:    
    // Update function called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Reduces character's health by the specified amount
    UFUNCTION(BlueprintCallable)
    void ReduceHealth(float Amount);

    // Reduces character's stamina and triggers regeneration delay
    UFUNCTION(BlueprintCallable)
    void ReduceStamina(float Amount);
    
    // Handles gradual stamina regeneration
    UFUNCTION(BlueprintCallable)
    void RegenStamina();

    // Enables stamina regeneration after delay period
    UFUNCTION()
    void EnableRegen();
};