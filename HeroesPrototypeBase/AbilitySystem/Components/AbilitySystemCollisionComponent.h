// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/HeroesAbilitySet.h"
#include "Components/ActorComponent.h"
#include "AbilitySystemCollisionComponent.generated.h"

USTRUCT(BlueprintType)
struct FAbilityVolumeEnteredActorInfo
{
	GENERATED_USTRUCT_BODY();

	TArray<FGameplayAbilitySpecHandle> AppliedAbilities;

	TArray<FActiveGameplayEffectHandle> AppliedEffects;
};

/**
 * 
 */
UCLASS(ClassGroup = "AbilitySystem", meta = (BlueprintSpawnableComponent))
class HEROESPROTOTYPEBASE_API UAbilitySystemCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

	// Physics volume.

public:

	/** Default constructor. */
	UAbilitySystemCollisionComponent();

	virtual void BeginPlay() override;

	/** Grants this volume's ability sets when another actor collides with this actor. */
	UFUNCTION()
	void OnActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Removes this volume's temporary ability sets when another actor stops colliding with this actor. */
	UFUNCTION()
	void OnActorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	// Ability system.

protected:

	/** The name of the component that will detect when to grant and remove ability sets to overlapping actors. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability System")
	FName CollisionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayEffect>> OngoingEffectsToApply;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayEffect>> PermanentEffectsToApply;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayAbility>> OngoingAbilitiesToGive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayAbility>> PermanentAbilitiesToGive;

	TMap<AActor*, FAbilityVolumeEnteredActorInfo> EnteredActorsInfoMap;
};
