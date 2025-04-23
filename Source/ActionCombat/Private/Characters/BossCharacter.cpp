// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/BossCharacter.h"
#include "Characters/StatsComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Combat/CombatComponent.h"
#include "Characters/MainCharacter.h"
#include "Components/CapsuleComponent.h"
#include "interfaces/MainPlayer.h"

/*
 * Implementation of the boss enemy character
 * This class manages:
 * - AI blackboard setup and state transitions
 * - Combat and rear attack behavior
 * - Rotation logic and death handling
 */

// Sets default values
ABossCharacter::ABossCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize character components
	StatsComp = CreateDefaultSubobject<UStatsComponent>(TEXT("Stats Component"));
	// Combat component manages attack execution and animations
	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
}

// Called when the game starts or when spawned
void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Cache the AI controller
	ControllerRef = GetController<AAIController>();

	// Access blackboard component and set initial state
	BlackboardComp = ControllerRef->GetBlackboardComponent();
	BlackboardComp->SetValueAsEnum(
		TEXT("CurrentState"),
		InitialState
	);

	// Bind to player death event to react accordingly
	GetWorld()->GetFirstPlayerController()
		->GetPawn<AMainCharacter>()
		->StatsComp->OnZeroHealthDelegate
		.AddDynamic(this, &ABossCharacter::HandlePlayerDeath);
	
}

// Called every frame
void ABossCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
	// Handle turning logic if boss is rotating toward target
    if (bIsTurning)
    {
        FRotator CurrentRotation = GetActorRotation();
        FRotator NewRotation = FMath::RInterpTo(
            CurrentRotation,
            TargetRotation,
            DeltaTime,
            TurnSpeed
        );
        
        SetActorRotation(NewRotation);
    	
    	// Stop turning once within acceptable angle threshold
        if (FMath::Abs(NewRotation.Yaw - TargetRotation.Yaw) < 1.0f)
        {
            bIsTurning = false;
        }
    }
	
	// Rear attack logic: detect if player is behind boss
    if (IsPlayerBehind())
    {
        if (!bIsPlayerBehind)
        {
        	// Player has just moved behind boss
            bIsPlayerBehind = true;
            TimeBehindBoss = 0.0f;
        }
        else
        {
        	// Player remains behind boss
            TimeBehindBoss += DeltaTime;
            if (TimeBehindBoss >= BehindCheckTime)
            {
                PerformRearAttack();
                TimeBehindBoss = 0.0f;
            }
        }
    }
    else
    {
        bIsPlayerBehind = false;
        TimeBehindBoss = 0.0f;
    }
}

// Called to bind functionality to input
void ABossCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Detects the player and changes AI state from idle to ranged
void ABossCharacter::DetectPawn(APawn* DetectedPawn, APawn* PawnToDetect)
{
	EEnemyState CurrentState {
		static_cast<EEnemyState>(BlackboardComp->GetValueAsEnum(TEXT("CurrentState")))
	};
	
	// Only proceed if the detected pawn matches and state is idle
	if (DetectedPawn != PawnToDetect || CurrentState != EEnemyState::Idle) { return; }

	BlackboardComp->SetValueAsEnum(
		TEXT("CurrentState"),
		EEnemyState::Range
		);
	
	//UE_LOG(LogTemp, Warning, TEXT("Player detected: %s"), *DetectedPawn->GetName());
}

// Returns boss's strength stat as damage value
float ABossCharacter::GetDamage()
{
	return StatsComp->Stats[EStat::Strength];
}

// Executes a random melee attack
void ABossCharacter::Attack()
{
	CombatComp->RandomAttack();


}
// Returns the duration of the current attack animation
float ABossCharacter::GetAnimDuration()
{
	return CombatComp->AnimDuration;
}

// Returns the melee attack range from stats
float ABossCharacter::GetMeleeRange()
{
	return StatsComp->Stats[EStat::MeleeRange];
}

// Handles logic when player dies (sets AI state to GameOver)
void ABossCharacter::HandlePlayerDeath()
{
	ControllerRef->GetBlackboardComponent()
		->SetValueAsEnum(
			TEXT("CurrentState"), EEnemyState::GameOver
		);
}

// Handles boss death: animation, AI logic stop, disable collisions, and cleanup
void ABossCharacter::HandleDeath()
{
	float Duration{ PlayAnimMontage(DeathAnim) };

	ControllerRef->GetBrainComponent()
		->StopLogic("defeated");

	FindComponentByClass<UCapsuleComponent>()
		->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FTimerHandle DestroyTimerHandle{};

	GetWorld()->GetTimerManager().SetTimer(
		DestroyTimerHandle,
		this,
		&ABossCharacter::FinishDeathAnim,
		Duration,
		false
	);

	IMainPlayer* PlayerRef{
		GetWorld()->GetFirstPlayerController()->GetPawn<IMainPlayer>()
	};

	if (!PlayerRef) { return; }

	PlayerRef->EndLockOnWithActor(this);
}

// Destroys the boss actor after death animation
void ABossCharacter::FinishDeathAnim()
{
	Destroy();
}

// Determines if the player is behind the boss based on angle
bool ABossCharacter::IsPlayerBehind() const
{
    APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
    if (!PlayerPawn) return false;

    // Get direction vectors
    FVector ToPlayer = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    FVector Forward = GetActorForwardVector();
    
    // Calculate angle between forward vector and direction to player
    float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Forward, ToPlayer)));
    
    // Return true if angle is greater than threshold
    return Angle > BehindAngleThreshold;
}


void ABossCharacter::PerformRearAttack()
{
	// Executes a random rear attack (tail swipe, turn, or jump)
	
    // Random choice between different reactions
    int32 ReactionType = FMath::RandRange(0, 1); // will need to change max to two if i add jump back
    
    switch (ReactionType)
    {
        case 0: // Tail swipe
            if (RearAttackMontage)
            {
                PlayAnimMontage(RearAttackMontage);
            }
            break;
            
        case 1: // Smooth turn toward player
        {
            APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
            if (PlayerPawn)
            {
                FVector Direction = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
                TargetRotation = Direction.Rotation();
                TargetRotation.Pitch = GetActorRotation().Pitch; // Keep current pitch
                bIsTurning = true;
            }
        }
        break;
            
        // case 2: // Jump back might add 
        //     {
        //         FVector BackwardDirection = -GetActorForwardVector();
        //         FVector JumpTarget = GetActorLocation() + (BackwardDirection * 500.0f); // Adjust distance as needed
        //         
        //         // ill need to implement the actual movement logic here//   
        //         LaunchCharacter(BackwardDirection * 500.0f, true, false);
        //     }
        //     break;
    }
}



