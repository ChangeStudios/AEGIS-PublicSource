// Copyright Samuel Reitich 2024.


#include "AbilitySystem/Tasks/AbilityTask_ServerWaitTargetData.h"

#include "AbilitySystemComponent.h"
#include "Editor.h"

UAbilityTask_ServerWaitTargetData* UAbilityTask_ServerWaitTargetData::ServerWaitForClientTargetData(UGameplayAbility* OwningAbility, FName TaskInstanceName)
{
	UAbilityTask_ServerWaitTargetData* MyTask = NewAbilityTask<UAbilityTask_ServerWaitTargetData>(OwningAbility, TaskInstanceName);
	return MyTask;
}

void UAbilityTask_ServerWaitTargetData::Activate()
{
	if (!Ability || !Ability->GetCurrentActorInfo()->IsNetAuthority())
	{
		return;
	}

	const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
	const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
	AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UAbilityTask_ServerWaitTargetData::OnTargetDataReplicatedCallback);
}

void UAbilityTask_ServerWaitTargetData::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag)
{
	// Valid target data that was replicated to the server for it to use.
	FGameplayAbilityTargetDataHandle MutableData = Data;
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(MutableData);
	}

	EndTask();
}

void UAbilityTask_ServerWaitTargetData::OnDestroy(bool AbilityEnded)
{
	if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		ASC->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).RemoveAll(this);
	}

	Super::OnDestroy(AbilityEnded);
}
