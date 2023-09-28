// Copyright Samuel Reitich 2023.

#pragma once

#include "Abilities/GameplayAbilityTargetActor.h"

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_HeroesWaitTargetData.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitTargetDataDelegate, const FGameplayAbilityTargetDataHandle&, Data);

/**
 * Waits to receive TargetData. This is used by the server to wait for clients to send it targeting data.
 *
 * The original UAbilityTask_WaitTargetData task creates and destroys a target data actor each time it is called, which
 * is unnecessary. Here, we only create one target data actor for each ability and re-use that actor each time the
 * ability is activated. This requires the ability to be instanced-per-actor; otherwise, the behavior will be identical
 * to Unreal's built-in WaitTargetData task.
 *
 * The UAbilityTask_WaitTargetData class is not well-suited for subclassing, so we do a full rewrite here.
 * 
 */
UCLASS()
class HEROESPROTOTYPEBASE_API UAbilityTask_HeroesWaitTargetData : public UAbilityTask
{
	GENERATED_BODY()

public:

	/**
	 * An optimized version of the basic WaitTargetData task. This task will use the given target actor to generate and
	 * return target data, without destroying the target actor. If a target actor is not given, one of the specified
	 * class will be created and returned.
	 *
	 * @param OwningAbility								The ability instance executing this task.
	 * @param TaskInstanceName							An optional name assigned to this ability task instance that can
	 *													later be used to
	 *													reference it.
	 * @param ConfirmationType							How this targeting information will be confirmed. For most
	 *													cases, like hitscan weapons, this should be "instant."
	 * @param InTargetActor								The target data actor being used by this ability. This should
	 *													always be cached in the
	 *													ability executing this task. If a nullptr is passed, then a new
	 *													actor of type <TargetActorClass> will be created and returned as
	 *													OutTargetActor.
	 * @param OutTargetActor							A reference to the newly created target actor, if InTargetActor
	 *													was null.
	 * @param TargetActorClass							The target actor class to create if InTargetActor is null.
	 * @param bCreateKeyIfNotValidForMorePredicting		Whether to create a new scoped prediction key if the current key
	 *													is no longer valid for predicting. Always creates a new scoped
	 *													prediction key by default. Set this to "false" to use an
	 *													existing key, like an ability's activation key for a batched
	 *													ability.
	 *
	 */
	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true", HideSpawnParms = "Instigator"), Category="Ability|Tasks")
	static UAbilityTask_HeroesWaitTargetData* WaitTargetDataWithReusableActor(UGameplayAbility* OwningAbility, FName TaskInstanceName, TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType, AGameplayAbilityTargetActor* InTargetActor, bool bCreateKeyIfNotValidForMorePredicting = true);

	/** Called when the ability is asked to confirm from an outside node. What this means depends on the individual
	 * task. By default, this does nothing other than ending if bEndTask is true. */
	virtual void ExternalConfirm(bool bEndTask) override;

	/** Called when the ability is asked to cancel from an outside node. What this means depends on the individual task.
	 * By default, this does nothing other than ending the task. */
	virtual void ExternalCancel() override;



	// Internal task functions.

// Task logic.
protected:

	/** Called when this task begins waiting to receive target data. */
	virtual void Activate() override;

	/** Called when this task is ended. */
	virtual void OnDestroy(bool bInOwnerFinished) override;

	/** Registers this task's ASC to the callbacks that are triggered when the targeting data is ready. */
	UFUNCTION()
	void RegisterTargetDataCallbacks();

	/** Determines if target data should be sent to the server by this task. Target data should only be sent to the
	 * server if this is the client and if this is not a GameplayTargetActor that can produce data on the server
	 * directly. */
	bool ShouldReplicateDataToServer() const;

// Target data actor.
protected:

	/** Configures this ability's target data actor for this activation of that ability. Because we re-use the same
	 * target data actor for each activation of the ability, we need to re-configure it each time. */
	virtual void ConfigureTargetActor();

	/** Sets up the target actor's targeting logic. If targeting should be instantly confirmed, it is confirmed here.
	 * Otherwise, registers to callbacks for when the targeting is confirmed or cancelled. */
	virtual void FinalizeTargetActorInitialization() const;

// Client callbacks.
protected:

	/** Called when the target data is ready to be sent to the server. */
	UFUNCTION()
	virtual void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& Data);

	/** Called when the client cancels targeting. */
	UFUNCTION()
	virtual void OnTargetDataCancelled(const FGameplayAbilityTargetDataHandle& Data);

// Server callbacks.
protected:

	/** Called when the target data is received by the server from a client. */
	UFUNCTION()
	virtual void OnTargetDataReplicated(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag);

	/** Called when the server receives that targeting was cancelled by a client. */
	UFUNCTION()
	virtual void OnTargetDataReplicatedCancelled();

// Delegates.
protected:

	/** Broadcasted when valid target data is sent to the server. */
	UPROPERTY(BlueprintAssignable)
	FWaitTargetDataDelegate ValidData;

	/** Broadcasted when targeting is cancelled. */
	UPROPERTY(BlueprintAssignable)
	FWaitTargetDataDelegate Cancelled;



	// Ability task parameters.

protected:

	/** The target actor being used for this task. This will be created the first time it's instanced and re-used for
	 * each ability activation afterwards. */
	UPROPERTY()
	TObjectPtr<AGameplayAbilityTargetActor> TargetActor;

	/** How this ability task will be confirmed. This should be "instant" in most cases, such as for hitscan weapons. It
	 * may be different for abilities that, for example, require manual confirmation by the player while targeting. */
	TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType;

	/** If true, this ability task will create a new scoped prediction key if the current key is no longer valid for
	 * predicting. Setting this to false enables this task's ability to use an existing key, such as an activation key
	 * for the ability if it was batched. */
	bool bCreateKeyIfNotValidForMorePredicting;
};
