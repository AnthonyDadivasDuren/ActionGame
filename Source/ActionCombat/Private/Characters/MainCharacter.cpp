// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/MainCharacter.h"
#include "Animations/PlayerAnimInstance.h"
#include "Characters/StatsComponent.h"
#include "Characters/EStat.h"
#include "Combat/LockOnComponent.h"
#include "Combat/CombatComponent.h"
#include "Combat/TraceComponent.h"
#include "Combat/BlockComponent.h"
#include "Characters/PlayerActionsComponent.h"

/*
 * Implementation of the main playable character
 * This class handles the core functionality of the player character including:
 * - Component initialization and management
 * - Combat mechanics (damage, blocking)
 * - Character states and animations
 * - Input processing
 */

// Sets default values
AMainCharacter::AMainCharacter()
{
    // Enable tick function for frame-by-frame updates
    PrimaryActorTick.bCanEverTick = true;

    // Initialize all core character components
    // Stats component handles character attributes (health, stamina, etc.)
    StatsComp = CreateDefaultSubobject<UStatsComponent>(TEXT("Stats Component"));
    // LockOn component manages target acquisition and tracking
    LockOnComp = CreateDefaultSubobject<ULockOnComponent>(TEXT("LockOn Component"));
    // Combat component handles attack mechanics and combat states
    CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
    // Trace component manages collision detection and hit registration
    TraceComp = CreateDefaultSubobject<UTraceComponent>(TEXT("Trace Component"));
    // Block component handles defensive mechanics
    BlockComp = CreateDefaultSubobject<UBlockComponent>(TEXT("Block Component"));
    // Player Actions component manages character movement states and special actions
    PlayerActionsComp = CreateDefaultSubobject<UPlayerActionsComponent>(TEXT("Player Actions Component"));
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Get and store reference to the animation instance for later use
    PlayerAnim = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    // Note: Input bindings set up in Blueprint
}

float AMainCharacter::GetDamage()
{
    // Returns the character's current strength stat as damage value
    return StatsComp->Stats[EStat::Strength];
}

bool AMainCharacter::HasEnoughStamina(float Cost)
{
    // Validates if the character has sufficient stamina for an action
    return StatsComp->Stats[EStat::Stamina] >= Cost;
}

void AMainCharacter::HandleDeath()
{
    // Plays death animation and disables player input
    PlayAnimMontage(DeathAnimMontage);
    DisableInput(GetController<APlayerController>());
}

void AMainCharacter::EndLockOnWithActor(AActor* ActorRef)
{
    // Ends lock-on only if the provided actor is the current target
    if (LockOnComp->CurrentTargetActor != ActorRef) { return; }
    LockOnComp->EndLockOn();
}

bool AMainCharacter::CanTakeDamage(AActor* Opponent)
{
    // Invulnerable while rolling
    if (PlayerActionsComp->bIsRollActive) { return false; }
    
    // If blocking, check for parry first
    if (PlayerAnim->bIsBlocking)
    {
        // Check for parry first
        if (BlockComp->AttemptParry(Opponent))
        {
            BlockComp->OnSuccessfulParry(Opponent);
            return false;
        }
        
        // Return true to allow damage calculation with reduction
        return true;
    }
    
    // Character can take full damage if not rolling or blocking
    return true;

}

void AMainCharacter::PlayHurtAnim(TSubclassOf<UCameraShakeBase> CameraShakeTemplate)
{
    // Plays hurt animation and applies camera shake effect if provided
    PlayAnimMontage(HurtAnimMontage);

    if (CameraShakeTemplate)
    {
        GetController<APlayerController>()->ClientStartCameraShake(CameraShakeTemplate);
    }
}

float AMainCharacter::CalculateReceivedDamage(float IncomingDamage, AActor* DamageCauser)
{
    // If blocking and block check returns false (successful block)
    if (PlayerAnim && PlayerAnim->bIsBlocking)
    {
        // BlockComp->Check returns false when block is successful
        if (!BlockComp->Check(DamageCauser))  // Block succeeded
        {
            return BlockComp->GetReducedDamage(IncomingDamage);
        }
    }
    
    // Return full damage if not blocking or if block failed
    return IncomingDamage;

    
}

float AMainCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
    class AController* EventInstigator, AActor* DamageCauser)
{
    if (!CanTakeDamage(DamageCauser))
    {
        return 0.0f;
    }

    float FinalDamage = CalculateReceivedDamage(DamageAmount, DamageCauser);
    
    // Apply the final damage through the parent class
    return Super::TakeDamage(FinalDamage, DamageEvent, EventInstigator, DamageCauser);

}
