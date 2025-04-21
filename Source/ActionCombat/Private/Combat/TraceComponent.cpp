// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/TraceComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Interfaces/Fighter.h"
#include "kismet/GameplayStatics.h"


// Sets default values for this component's properties
UTraceComponent::UTraceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
}



// Called when the game starts
void UTraceComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get a reference to the skeletal mesh component of the owning actor
	SkeletalComp = GetOwner()
		->FindComponentByClass<USkeletalMeshComponent>();

	
	
}


// Called every frame
void UTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// Skip if not currently attacking
	if (!bIsAttacking) { return; }
	
	TArray<FHitResult> AllResults;
	
	for (const FTraceSockets Socket: Sockets)
	{
		// Get socket locations and rotation for the trace
		FVector StartSocketLocation {
			SkeletalComp->GetSocketLocation(Socket.Start) };
		FVector EndSocketLocation {
			SkeletalComp->GetSocketLocation(Socket.End) };
		FQuat ShapeRotation {
			SkeletalComp->GetSocketQuaternion(Socket.Rotation) };
	
		TArray<FHitResult> OutResults;

		// Calculate the distance between the sockets and create a box shape
		double WeaponDistance {
			FVector::Distance(StartSocketLocation, EndSocketLocation)
		};
		FVector BoxHalfExtent {
			BoxCollisionLength, BoxCollisionLength, WeaponDistance
		};
		BoxHalfExtent /= 2; // BoxHalfExtent = BoxHalfExtent / 2
		FCollisionShape Box{
			FCollisionShape::MakeBox(BoxHalfExtent)
		};

		// Set up collision query to ignore the owner
		FCollisionQueryParams IgnoreParams {
			FName { TEXT("Ignore Params") },
			false,
			GetOwner(),
		};
	
		// Perform a sweep using a box shape to find overlapping actors
		bool bHasFoundTargets  { GetWorld()->SweepMultiByChannel(
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


		// Optional debug drawing of the trace box
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
				bHasFoundTargets ? FLinearColor::Green : FLinearColor::Red, // More Compact If statement if ? Then : else
				ShapeRotation.Rotator(),
				1.0f,
				2.0f
			);
		}
	}


	//Debug to See if the weapon Found a target when touched
	// if (bHasFoundTargets)
	// {
	// 	UE_LOG(
	// 		LogTemp,
	// 		Warning,
	// 		TEXT("Target Found!")
	// 	);
	// }


	//Debug to See that the Weapon parts locations is found and logged
	// UE_LOG(
	// 	LogTemp,
	// 	Warning,
	// 	TEXT("Start: %s - End: %s - Rotation: %s"),
	// 	*StartSocketLocation.ToString(),
	// 		*EndSocketLocation.ToString(),
	// 		*ShapeRotation.ToString()
	// 	);

	
	// Skip if no hits
	if (AllResults.Num() == 0) { return; }

	// Determine how much damage to apply
	float CharacterDamage{ 0.0f };

	IFighter* FighterRef{ Cast<IFighter>(GetOwner()) };

	if (FighterRef){
		CharacterDamage = FighterRef->GetDamage();
		
	}

	// UE_LOG(LogTemp, Warning, TEXT("Damage: %f"), CharacterDamage);

	// Create a generic damage event
	FDamageEvent TargetAttackedEvent;

	// Loop through all hit results
	for (const FHitResult& Hit : AllResults)
	{
		AActor* TargetActor{ Hit.GetActor() };

		// Skip actors already hit during this trace
		if (TargetsToIgnore.Contains(TargetActor)) { continue; }

		// Apply damage to the hit actor
		TargetActor -> TakeDamage(
			CharacterDamage,
			TargetAttackedEvent,
			GetOwner()->GetInstigatorController(),
			GetOwner()
		);
		
		// Add to ignore list so we don't damage them again this frame
		TargetsToIgnore.AddUnique(TargetActor);

		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticleTemplate,
			Hit.ImpactPoint
		);
	}
}

// Clears the list of targets hit during this attack (at the end of the animation)
void UTraceComponent::HandleResetAttack()
{
	TargetsToIgnore.Empty();
}


