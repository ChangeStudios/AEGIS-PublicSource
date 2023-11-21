// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ProjectileExtensionComponent.generated.h"

class UGameplayEffect;

/**
 * The method by which a projectile travels.
 */
UENUM(BlueprintType)
enum class EProjectileMovementStyle : uint8
{
	// This projectile travels using the movement values described in its projectile component.
	Default = 0,
	// This projectile follows the aim of the owning player until activated.
	PlayerGuided,
	// This projectile travels indefinitely with default movement but can be manually stopped by the player.
	PlayerStopped
};

/**
 * TODO: Add an option to exclusively activate projectiles on the ground (as opposed to walls or other actors). This
 * might require explicitly marking surfaces that count as "ground" on each map to ensure consistency.
 */
UENUM(BlueprintType)
enum class EProjectileActivationStyle : uint8
{
	// This projectile does not have any kind of activation.
	NoActivation = 0,
	/* The projectile will activate upon impacting any actor. If this projectile can bounce, it will activate once it
	 * impacts an actor after bouncing the maximum number of times. */
	OnImpactAny,
	/* The projectile will activate upon impacting a valid target. If this projectile can bounce, it will stop bouncing
	 * and activate once it impacts a valid target. If the projectile impacts an invalid target and is out of bounces,
	 * it will activate anyway. */
	OnImpactTarget,
	// This projectile has a timer that begins when it spawns. When the timer ends, the projectile activates.
	Timed,
	/* This projectile has a timer that begins upon impacting any actor. When the timer ends, the projectile
	 * activates. */
	TimedAfterInitialImpact,
	/* This projectile has a timer that begins upon impacting any actor after bouncing the maximum number of times.
	 * When the timer ends, the projectile activates. */
	TimedAfterFinalImpact,
	// This projectile will activate once it has stopped moving.
	AfterMovementStops,
	// This projectile will activate after traveling a fixed distance.
	AfterFixedDistance
};

/**
 * Defines the alignment of another actor with respect to the player.
 *
 * TODO: Move this to a different file once teams are implemented.
 */
UENUM(BlueprintType)
enum class ERelativeTeamAlignment : uint8
{
	// Any player on an opposing team.
	Enemy,
	// Any player on the local player's team.
	Ally,
	// The local player.
	Self
};

/**
 * A collection of gameplay effects and their corresponding types of targets.
 */
USTRUCT(BlueprintType)
struct FTargetedEffects
{

	GENERATED_BODY()

public:

	/** The effect to apply. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayEffect>> Effects;

	/** The types of targets to whom the effect will be applied. The targets themselves are determined by the
	 * projectile's targeting method. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<ERelativeTeamAlignment> Targets;
};

/**
 * Defines how a projectile will find its targets upon activation.
 */
UENUM(BlueprintType)
enum class EProjectileTargetingMethod : uint8
{
	// Actors directly impacted by the projectile will be targeted.
	DirectImpact,
	// Actors within a defined volume of the projectile and with line-of-sight to the projectile will be targeted.
	InVolumeWithLOS,
	// Actors within a defined volume of the projectile will be targeted, regardless of LoS.
	InVolumeWithoutLOS
};

/**
 * An extension of the default projectile movement component. This includes additional movement methods, functionality
 * for activation and targeting, and integration with the gameplay abilities system.
 */
UCLASS(ClassGroup = Movement, meta = (BlueprintSpawnableComponent), ShowCategories = (Velocity), DisplayName = "Enhanced Projectile Movement")
class HEROESPROTOTYPEBASE_API UProjectileExtensionComponent : public UProjectileMovementComponent
{
	GENERATED_BODY()

	/** Delegate fired when the projectile is activated. */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProjectileActivationDelegate, const FHitResult&, Hit, TArray<AActor*>, Targets);

	// Construction.

public:

	/** Default constructor. */
	UProjectileExtensionComponent(const FObjectInitializer& ObjectInitializer);



	// Initialization.

public:

	/** Caches various components from the owning actor. Doing this in BeginPlay ensures all of the actor's components
	 * are initialized. */
	virtual void BeginPlay() override;



	// Utils.

protected:

	/** A cached pointer to this component's owning actor: the projectile itself. */
	UPROPERTY(BlueprintReadOnly, Category = "Heroes|Projectiles")
	TObjectPtr<AActor> Projectile = nullptr;

	/** A typed pointer to the UpdatedComponent, which handles collision for this projectile. This should be the owning
	 * actor's root component. */
	UPROPERTY(BlueprintReadOnly, Category = "Heroes|Projectiles")
	TObjectPtr<UShapeComponent> ProjectileCollisionComponent = nullptr;



	// Movement.

protected:

	/** The method by which this projectile travels. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Projectile, meta = (DisplayPriority = 0))
	EProjectileMovementStyle ProjectileMovementStyle;



	// Bouncing.

// Bounce behavior.
protected:

	/** If this is true and bouncing is enabled, this projectile will bounce until it has bounced the maximum number of
	 * times. If this is false, this projectile will bounce indefinitely. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ProjectileBounces, meta = (DisplayAfter = "bShouldBounce"))
	bool bLimitBounces = false;

	/** The maximum number of times this projectile will bounce, if bLimitBounces is true. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ProjectileBounces, DisplayName = "Maximum Number of Bounces", meta = (DisplayAfter = "bLimitBounces", EditCondition = "bLimitBounces", ClampMin = "1"))
	int MaxBounces = 1;

// Internal bounce logic.
protected:

	/** Counter tracking how many times this projectile has bounced. */
	UPROPERTY(BlueprintReadOnly, Category = "Heroes|Projectiles")
	int CurrentBounceCount = 0;



	// Activation.

// Detecting activation.
protected:

	/** Called when this projectile impacts an actor while traveling. This can result in a bounce or an activation,
	 * depending on the projectile's bounce and activation behavior. */
	UFUNCTION()
	void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** A pointer to the collision component used as the volume for detecting targets. This is only used if the
	 * targeting method uses a volume. */
	UPROPERTY(BlueprintReadOnly, Category = "Heroes|Projectiles")
	TObjectPtr<UShapeComponent> TargetingVolume = nullptr;

	/** Tracks whether or not the projectile is active. Once a projectile has been activated, it cannot be activated
	 * again. There should be no way for projectiles to activate a second time; this is an additional check for
	 * safety. */
	UPROPERTY(BlueprintReadOnly, Category = "Heroes|Projectiles")
	bool bProjectileIsActive = false;

	/** Handle for the timer that activates this projectile, if this projectile has a timer-based activation style. */
	FTimerHandle ActivationTimerHandle;

	/** Called when this projectile stops moving to activate projectiles using the AfterMovementStops activation
	 * style. */
	UFUNCTION()
	void OnMovementStopped(const FHitResult& ImpactResult);

// Activation logic.
protected:

	/** Called to activate this projectile. Finds targets depending on the projectile's targeting method and passes
	 * them into the OnProjectileActivation function and delegate. */
	void ActivateProjectile(const FHitResult& Hit);

	/** Called when this projectile is activated. Applies given gameplay effects to their corresponding targets.
	 * Override this for custom projectile functionality. */
	virtual void OnProjectileActivation_Internal(const FHitResult& Hit, TArray<AActor*> Targets);

	/** Delegate fired when this projectile is activated. This allows for custom activation logic to be implemented in
	 * blueprints. */
	UPROPERTY(BlueprintAssignable, DisplayName = "OnProjectileActivation")
	FOnProjectileActivationDelegate B_OnProjectileActivation;

// Activation style.
protected:

	/** How this projectile "activates." */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectileActivation")
	EProjectileActivationStyle ProjectileActivationStyle;

	/** The duration of this projectile's activation timer. Projectiles with timer-based activation will activate at
	 * the end of the timer's duration. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectileActivation", meta = (DisplayAfter = "ProjectileActivationStyle", EditCondition = "ProjectileActivationStyle == EProjectileActivationStyle::Timed || ProjectileActivationStyle == EProjectileActivationStyle::TimedAfterInitialImpact || ProjectileActivationStyle == EProjectileActivationStyle::TimedAfterFinalImpact", ClampMin = "0.0"))
	float TimerDuration = 0.0f;

	/** Which targets will cause this projectile to activate, if it activates by impacting a target. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectileActivation", meta = (EditCondition = "ProjectileActivationStyle == EProjectileActivationStyle::OnImpactTarget"))
	TArray<ERelativeTeamAlignment> ValidImpactTargets;

	/** A collection of effects to apply when this projectile is activated. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectileActivation", DisplayName = "Effects Applied on Activation")
	TArray<FTargetedEffects> ActivationEffects;

	/** The method in which this projectile will find targets upon activation. Effects will automatically be applied to
	 * valid targets and the targets are passed to virtual activation functions for custom use. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectileActivation")
	EProjectileTargetingMethod ProjectileTargetingMethod;

};
