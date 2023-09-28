// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_HeroesWaitTargetData.h"
#include "AbilityTask_ServerWaitTargetData.generated.h"

/**
 * Instructs the server to wait to receive target data from the client.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API UAbilityTask_ServerWaitTargetData : public UAbilityTask
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FWaitTargetDataDelegate	ValidData;

	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true", HideSpawnParms = "Instigator"), Category = "Ability|Tasks")
	static UAbilityTask_ServerWaitTargetData* ServerWaitForClientTargetData(UGameplayAbility* OwningAbility, FName TaskInstanceName);

	virtual void Activate() override;

	UFUNCTION()
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag);

protected:

	virtual void OnDestroy(bool AbilityEnded) override;

};
