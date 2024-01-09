// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "HeroesGATA_Trace.generated.h"

class UWeaponItemTrait;

/**
 * A reusable and re-configurable trace target actor. Subclass this with custom trace shapes.
 *
 * This should be used with the HeroesWaitTargetData ability task. The default WaitTargetData task will destroy the
 * target actor, preventing this from being reused.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API AHeroesGATA_Trace : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void Configure
	(
		UPARAM(DisplayName = "Source Actor") AActor* InSourceActor,
		UPARAM(DisplayName = "Trace Profile") FCollisionProfileName InTraceProfile,
		UPARAM(DisplayName = "Ignore Blocking Hits") bool bInIgnoreBlockingHits = false,
		UPARAM(DisplayName = "Should Produce Target Data on Server") bool bInShouldProduceTargetDataOnServer = false,
		UPARAM(DisplayName = "Max Range") float InMaxRange = 999999.0f,
		UPARAM(DisplayName = "Weapon Item Trait") UWeaponItemTrait* InWeaponItemTrait = nullptr
	);

public:

	virtual void ConfirmTargetingAndContinue() override;

	virtual void CancelTargeting() override;

protected:

	virtual TArray<FHitResult> PerformTrace(AActor* InSourceActor);

public:

	UPROPERTY(BlueprintReadOnly)
	FVector CurrentTraceEnd = FVector::ZeroVector;

protected:

	FGameplayAbilityTargetingLocationInfo StartLocation;
	FCollisionProfileName TraceProfile;
	bool bIgnoreBlockingHits;
	bool bShouldProduceTargetDataOnServer;
	float MaxRange;

	UPROPERTY()
	UWeaponItemTrait* WeaponItemTrait;
};
