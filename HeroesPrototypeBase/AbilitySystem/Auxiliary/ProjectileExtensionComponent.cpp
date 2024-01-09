// Copyright Samuel Reitich 2024.


#include "AbilitySystem/Auxiliary/ProjectileExtensionComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "HeroesLogChannels.h"
#include "Components/ShapeComponent.h"
#include "HeroesGameFramework/HeroesGameplayStatics.h"

UProjectileExtensionComponent::UProjectileExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UProjectileExtensionComponent::BeginPlay()
{
	// Cache the projectile actor using this component.
	Projectile = IsValid(GetOwner()) ? GetOwner() : nullptr;
	check(Projectile);

	// Ensure that this projectile was given a player instigator.
	if (!Projectile->GetInstigator())
	{
		UE_LOG(LogHeroes, Error, TEXT("UProjectileExtensionComponent: Projectile actor [%s] was not spawned with a valid instigator. The instigator must be the player that spawned it."), *GetNameSafe(Projectile.Get()));
	}

	/* Retrieve the UpdatedComponent (the root component) as a collision component and cache it. Doing this in
	 * BeginPlay ensures that the owning actor's components have been initialized. */
	ProjectileCollisionComponent = Cast<UShapeComponent>(UpdatedComponent);
	ensure(ProjectileCollisionComponent);

	// Projectiles should never collide with their owner.
	ProjectileCollisionComponent->IgnoreActorWhenMoving(Projectile->GetInstigator(), true);
	Projectile->GetInstigator()->MoveIgnoreActorAdd(Projectile.Get());
	ProjectileCollisionComponent->MoveIgnoreActors.Add(Projectile->GetInstigator());

	// Assign a callback to when this projectile hits something, so we can perform logic like bouncing and activation.
	ProjectileCollisionComponent->OnComponentHit.AddDynamic(this, &UProjectileExtensionComponent::OnProjectileHit);


	// If this projectile uses a volume to detect targets upon activation, retrieve and cache that volume component.
	if (ProjectileTargetingMethod == EProjectileTargetingMethod::InVolumeWithLOS ||
		ProjectileTargetingMethod == EProjectileTargetingMethod::InVolumeWithoutLOS)
	{
		TArray<UActorComponent*> ShapeComponents;
		Projectile->GetComponents(UShapeComponent::StaticClass(), ShapeComponents);
		for (UActorComponent* ShapeComponent : ShapeComponents)
		{
			// The volume component cannot be the same as the projectile's collision component.
			if (ShapeComponent != ProjectileCollisionComponent)
			{
				TargetingVolume = Cast<UShapeComponent>(ShapeComponent);
			}
		}
	
		if (!TargetingVolume)
		{
			UE_LOG(LogTemp, Error, TEXT("UProjectileExtensionComponent: Projectile [%s] uses a volume for targeting, but does not have a collision volume that is not the projectile collision component."), *GetNameSafe(Projectile->GetClass()));
		}
	}

	// If this projectile activates with a timer, start the timer when it is spawned.
	if (ProjectileActivationStyle == EProjectileActivationStyle::Timed)
	{
		GetOwner()->GetWorldTimerManager().SetTimer(ActivationTimerHandle, FTimerDelegate::CreateLambda([&]
		{
			// Activate the projectile without a hit result when the timer ends.
			ActivateProjectile(FHitResult());
			GetOwner()->GetWorldTimerManager().ClearTimer(ActivationTimerHandle);
		}), TimerDuration, false);
	}
	// If this projectile activates when it stops moving, register an event to when the projectile stops.
	else if (ProjectileActivationStyle == EProjectileActivationStyle::AfterMovementStops)
	{
		OnProjectileStop.AddDynamic(this, &UProjectileExtensionComponent::OnMovementStopped);
	}

	Super::BeginPlay();
}

void UProjectileExtensionComponent::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// This function is called before the projectile bounces.
	// If this projectile has bounced the maximum number of times BEFORE this hit, disable bouncing.
	if (bShouldBounce && bLimitBounces)
	{
		if (CurrentBounceCount >= MaxBounces)
		{
			bShouldBounce = false;
		}
		else
		{
			CurrentBounceCount++;
		}
	}

	// If this projectile activates upon hitting an actor and is not bouncing, activate it.
	if (ProjectileActivationStyle == EProjectileActivationStyle::OnImpactAny)
	{
		if (!bShouldBounce)
		{
			ActivateProjectile(Hit);
			return;
		}
	}

	// If this projectile activates with a timer that begins upon hitting an actor and it is not bouncing, begin its activation timer.
	if (ProjectileActivationStyle == EProjectileActivationStyle::TimedAfterFinalImpact)
	{
		if (!bShouldBounce)
		{
			GetOwner()->GetWorldTimerManager().SetTimer(ActivationTimerHandle, FTimerDelegate::CreateLambda([&]
			{
				// Activate the projectile using the initial hit result when the timer ends.
				ActivateProjectile(Hit);
				GetOwner()->GetWorldTimerManager().ClearTimer(ActivationTimerHandle);
			}), TimerDuration, false);
		}
	}

	// If this projectile activates with a timer that begins upon hitting an actor, regardless of its bouncing behavior, begin its activation timer.
	if (ProjectileActivationStyle == EProjectileActivationStyle::TimedAfterInitialImpact)
	{
		// Only activate the timer once.
		if (!GetOwner()->GetWorldTimerManager().IsTimerActive(ActivationTimerHandle))
		{
			GetOwner()->GetWorldTimerManager().SetTimer(ActivationTimerHandle, FTimerDelegate::CreateLambda([&]
			{
				// Activate the projectile using the initial hit result when the timer ends.
				ActivateProjectile(Hit);
				GetOwner()->GetWorldTimerManager().ClearTimer(ActivationTimerHandle);
			}), TimerDuration, false);
		}
	}

	// If this projectile activates upon hitting an actor that is a valid target and the projectile hit a valid target, activate the projectile.
	if (ProjectileActivationStyle == EProjectileActivationStyle::OnImpactTarget)
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			if (UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitActor))
			{
				// TODO: Check this against the hit actor's team alignment instead of just the Enemy enum.
				if (ValidImpactTargets.Contains(ERelativeTeamAlignment::Enemy))
				{
					ActivateProjectile(Hit);
					return;
				}
			}
		}

		// If this projectile impacts an invalid target but is out of bounces, activate it.
		if (!bShouldBounce)
		{
			ActivateProjectile(Hit);
			return;
		}
	}
}

void UProjectileExtensionComponent::OnMovementStopped(const FHitResult& ImpactResult)
{
	// Activate this projectile if it should activate when movement stops.
	if (ProjectileActivationStyle == EProjectileActivationStyle::AfterMovementStops)
	{
		ActivateProjectile(ImpactResult);
	}
}

void UProjectileExtensionComponent::ActivateProjectile(const FHitResult& Hit)
{
	// Mark this projectile as "activated" so it can't be activated again.
	bProjectileIsActive = true;

	// Deactivate the projectile's collision component to stop registering hits.
	ProjectileCollisionComponent->OnComponentHit.RemoveAll(this);
	ProjectileCollisionComponent->Deactivate();

	// Initialize the array of targets that will be passed into our OnActivation functions.
	TArray<AActor*> Targets = TArray<AActor*>();

	// If the projectile targets the actor it impacts, pass in the hit actor as the target.
	if (ProjectileTargetingMethod == EProjectileTargetingMethod::DirectImpact)
	{
		Targets = TArray<AActor*>();
		if (IsValid(Hit.GetActor()))
		{
			Targets.Add(Hit.GetActor());
		}
	}
	// If the projectile targets actors within a defined volume, pass in all valid actors inside that volume as targets.
	else if (ProjectileTargetingMethod == EProjectileTargetingMethod::InVolumeWithLOS ||
		ProjectileTargetingMethod == EProjectileTargetingMethod::InVolumeWithoutLOS)
	{
		// Retrieve all actors within the projectile's defined effective volume.
		TArray<AActor*> OverlappingActors;
		TargetingVolume->GetOverlappingActors(OverlappingActors);

		for (AActor* OverlappingActor : OverlappingActors)
		{
			if (IsValid(OverlappingActor) && OverlappingActor != Projectile)
			{
				// If we don't need line-of-sight, add each actor to the list of targets.
				if (ProjectileTargetingMethod == EProjectileTargetingMethod::InVolumeWithoutLOS)
				{
					Targets.Add(OverlappingActor);
				}
				// If we need line-of-sight, test LoS to each actor before adding it to the list of targets.
				else if (ProjectileTargetingMethod == EProjectileTargetingMethod::InVolumeWithLOS)
				{
					UE_LOG(LogHeroes, Warning, TEXT("Testing for LoS: [%s]"), *GetNameSafe(OverlappingActor));
					const TArray<AActor*> ActorsToIgnore = TArray<AActor*>();
					if (UHeroesGameplayStatics::CanReachTarget(GetWorld(), Projectile->GetActorLocation(), OverlappingActor, ActorsToIgnore))
					{
						Targets.Add(OverlappingActor);
						UE_LOG(LogHeroes, Warning, TEXT("LoS success: [%s]"), *GetNameSafe(OverlappingActor));
					}
					else
					{
						UE_LOG(LogHeroes, Warning, TEXT("LoS fail: [%s]"), *GetNameSafe(OverlappingActor));
					}
				}
			}
		}

		// Deactivate the targeting volume to stop registering overlaps.
		TargetingVolume->Deactivate();
	}

	// Call any custom activation logic.
	OnProjectileActivation_Internal(Hit, Targets);
	B_OnProjectileActivation.Broadcast(Hit, Targets);

	// Deactivate the projectile movement component to stop the projectile's movement.
	Deactivate();
}

void UProjectileExtensionComponent::OnProjectileActivation_Internal(const FHitResult& Hit, TArray<AActor*> Targets)
{
	const UAbilitySystemComponent* OwnerASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Projectile->GetInstigator());
	if (!OwnerASC && ActivationEffects.Num() > 0)
	{
		UE_LOG(LogHeroes, Error, TEXT("UProjectileExtensionComponent: Owner [%s] does not have an ASC. Effects cannot be applied."), *GetNameSafe(Projectile.Get()));
		return;
	}

	for (AActor* Target : Targets)
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target))
		{
			for (FTargetedEffects TargetedEffect : ActivationEffects)
			{
				// TODO: Check this against the hit actor's team alignment instead of just the Enemy enum.
				if (TargetedEffect.Targets.Contains(ERelativeTeamAlignment::Enemy))
				{
					const FGameplayEffectContextHandle EffectContextHandle = OwnerASC->MakeEffectContext();
					for (TSubclassOf<UGameplayEffect> GameplayEffect : TargetedEffect.Effects)
					{
						if (!GameplayEffect.Get())
						{
							continue;
						}

						FGameplayEffectSpecHandle EffectSpecHandle = OwnerASC->MakeOutgoingSpec(GameplayEffect, 1, EffectContextHandle);
						if (EffectSpecHandle.IsValid())
						{
							TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
						}
					}
				}
			}
		}
	}
}