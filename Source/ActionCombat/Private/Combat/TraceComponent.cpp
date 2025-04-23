// Fill out your copyright notice in the Description page of Project Settings.

#include "Combat/TraceComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Interfaces/Fighter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UTraceComponent::UTraceComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UTraceComponent::BeginPlay()
{
    Super::BeginPlay();
    SkeletalComp = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
}

void UTraceComponent::SpawnHitEffect(const FVector& Location, EHitEffectType HitType)
{
    UParticleSystem* ParticleToSpawn = nullptr;
    
    switch(HitType)
    {
        case EHitEffectType::Block:
            ParticleToSpawn = BlockParticleTemplate;
            break;
        case EHitEffectType::Parry:
            ParticleToSpawn = ParryParticleTemplate;
            break;
        case EHitEffectType::Normal:
        default:
            ParticleToSpawn = HitParticleTemplate;
            break;
    }
    
    if (ParticleToSpawn)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            ParticleToSpawn,
            Location,
            FRotator::ZeroRotator,  // Default rotation
            FVector(1.0f),          // Default scale
            true            // Auto destroy when complete
        );
    }
}

void UTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (!bIsAttacking) { return; }
    
    TArray<FHitResult> AllResults;
    
    for (const FTraceSockets Socket: Sockets)
    {
        FVector StartSocketLocation { SkeletalComp->GetSocketLocation(Socket.Start) };
        FVector EndSocketLocation { SkeletalComp->GetSocketLocation(Socket.End) };
        FQuat ShapeRotation { SkeletalComp->GetSocketQuaternion(Socket.Rotation) };
    
        TArray<FHitResult> OutResults;

        double WeaponDistance { FVector::Distance(StartSocketLocation, EndSocketLocation) };
        FVector BoxHalfExtent { BoxCollisionLength, BoxCollisionLength, WeaponDistance };
        BoxHalfExtent /= 2;
        FCollisionShape Box { FCollisionShape::MakeBox(BoxHalfExtent) };

        FCollisionQueryParams IgnoreParams {
            FName { TEXT("Ignore Params") },
            false,
            GetOwner(),
        };
    
        bool bHasFoundTargets { GetWorld()->SweepMultiByChannel(
            OutResults,
            StartSocketLocation,
            EndSocketLocation,
            ShapeRotation,
            ECollisionChannel::ECC_GameTraceChannel1,
            Box,
            IgnoreParams
        ) };

        for (FHitResult& Hit : OutResults) {
            AllResults.Add(Hit);
        }

        if (bDebugMode)
        {
            FVector CenterPoint{
                UKismetMathLibrary::VLerp(
                    StartSocketLocation, EndSocketLocation, 0.5f
                )
            };
            
            UKismetSystemLibrary::DrawDebugBox(
                GetWorld(),
                CenterPoint,
                Box.GetExtent(),
                bHasFoundTargets ? FLinearColor::Green : FLinearColor::Red,
                ShapeRotation.Rotator(),
                1.0f,
                2.0f
            );
        }
    }

    if (AllResults.Num() == 0) { return; }

    float CharacterDamage{ 0.0f };
    IFighter* FighterRef{ Cast<IFighter>(GetOwner()) };
    if (FighterRef) {
        CharacterDamage = FighterRef->GetDamage();
    }

    FDamageEvent TargetAttackedEvent;

    for (const FHitResult& Hit : AllResults)
    {
        AActor* TargetActor{ Hit.GetActor() };

        // Skip if we've already processed this actor
        if (TargetsToIgnore.Contains(TargetActor)) { continue; }

        // Determine hit effect type based on target's state
        EHitEffectType HitType = EHitEffectType::Normal;
        IFighter* TargetFighter = Cast<IFighter>(TargetActor);
        
        if (TargetFighter)
        {
            if (TargetFighter->IsBlocking())
            {
                if (TargetFighter->IsParrying())
                {
                    HitType = EHitEffectType::Parry;
                }
                else if (TargetFighter->IsBlockFailed())
                {
                    HitType = EHitEffectType::Normal;  // Use blood effect for failed block
                }
                else
                {
                    HitType = EHitEffectType::Block;
                }
            }
        }

        // Only spawn effect once per hit actor
        SpawnHitEffect(Hit.ImpactPoint, HitType);

        
        TargetActor->TakeDamage(
            CharacterDamage,
            TargetAttackedEvent,
            GetOwner()->GetInstigatorController(),
            GetOwner()
        );
        
        TargetsToIgnore.AddUnique(TargetActor);
    }
}

void UTraceComponent::HandleResetAttack()
{
    TargetsToIgnore.Empty();
}