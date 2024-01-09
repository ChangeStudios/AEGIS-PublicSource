// Copyright Samuel Reitich 2024.


#include "AbilitySystem/Tasks/AbilityTask_HeroesWaitTargetData.h"

#include "AbilitySystemComponent.h"

UAbilityTask_HeroesWaitTargetData* UAbilityTask_HeroesWaitTargetData::WaitTargetDataWithReusableActor(UGameplayAbility* OwningAbility, FName TaskInstanceName, TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType, AGameplayAbilityTargetActor* InTargetActor, bool bCreateKeyIfNotValidForMorePredicting)
{
	if (!IsValid(InTargetActor))
	{
		return nullptr;
	}

	// Register this ability task.
	UAbilityTask_HeroesWaitTargetData* AbilityTask = NewAbilityTask<UAbilityTask_HeroesWaitTargetData>(OwningAbility, TaskInstanceName);

	// Cache the given parameters for this task.
	AbilityTask->TargetActor = InTargetActor;
	AbilityTask->ConfirmationType = ConfirmationType;
	AbilityTask->bCreateKeyIfNotValidForMorePredicting = bCreateKeyIfNotValidForMorePredicting;

	return AbilityTask;
}

void UAbilityTask_HeroesWaitTargetData::ExternalConfirm(bool bEndTask)
{
	if (TargetActor)
	{
		if (TargetActor->ShouldProduceTargetData())
		{
			TargetActor->ConfirmTargetingAndContinue();
		}
	}

	Super::ExternalConfirm(bEndTask);
}

void UAbilityTask_HeroesWaitTargetData::ExternalCancel()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		Cancelled.Broadcast(FGameplayAbilityTargetDataHandle());
	}

	Super::ExternalCancel();
}

void UAbilityTask_HeroesWaitTargetData::Activate()
{
	if (!IsValid(this))
	{
		return;
	}

	// Set up the target data actor to collect and send the targeting data.
	if (Ability && TargetActor)
	{
		// Configure the target data for this ability activation.
		ConfigureTargetActor();
		// Register to when the targeting data is received.
		RegisterTargetDataCallbacks();
		// Set up the targeting data actor's confirmation logic.
		FinalizeTargetActorInitialization();
	}
	/* If the ability or target data actor are invalid, don't perform any task logic. This should never happen. */
	else
	{
		EndTask();
	}
}

void UAbilityTask_HeroesWaitTargetData::OnDestroy(bool bInOwnerFinished)
{
	if (TargetActor)
	{
		// Call any target actor-specific StopTargeting functions here.

		TargetActor->SetActorTickEnabled(false);

		// Unbind this task's callbacks from the target actor.
		TargetActor->TargetDataReadyDelegate.RemoveAll(this);
		TargetActor->CanceledDelegate.RemoveAll(this);

		// Unbind the target actor's callbacks from this task.
		if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
		{
			ASC->GenericLocalConfirmCallbacks.RemoveDynamic(TargetActor, &AGameplayAbilityTargetActor::ConfirmTargeting);
			ASC->GenericLocalCancelCallbacks.RemoveDynamic(TargetActor, &AGameplayAbilityTargetActor::CancelTargeting);
			TargetActor->GenericDelegateBoundASC = nullptr;
		}
	}
	
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_HeroesWaitTargetData::RegisterTargetDataCallbacks()
{
	if (!ensure(IsValid(this)))
	{
		return;
	}

	check(Ability);

	const bool bLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	const bool bShouldProduceTargetDataOnServer = TargetActor->ShouldProduceTargetDataOnServer;

	// Prepare the server to receive the targeting data from the client.
	if (!bLocallyControlled)
	{
		if (!bShouldProduceTargetDataOnServer)
		{
			FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
			FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();

			// Register for when the client sends or cancels the targeting data.
			AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UAbilityTask_HeroesWaitTargetData::OnTargetDataReplicated);
			AbilitySystemComponent->AbilityTargetDataCancelledDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UAbilityTask_HeroesWaitTargetData::OnTargetDataReplicatedCancelled);

			// Check if the target data was already sent.
			AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);

			// Start waiting to receive the target data.
			SetWaitingOnRemotePlayerData();
		}
	}
}

bool UAbilityTask_HeroesWaitTargetData::ShouldReplicateDataToServer() const
{
	if (!Ability || !TargetActor)
	{
		return false;
	}

	// Only send target data if this is the client and is not a GameplayTargetActor that produces data on the server directly.
	const FGameplayAbilityActorInfo* Info = Ability->GetCurrentActorInfo();
	if (!Info->IsNetAuthority() && !TargetActor->ShouldProduceTargetDataOnServer)
	{
		return true;
	}

	return false;
}

void UAbilityTask_HeroesWaitTargetData::ConfigureTargetActor()
{
	check(TargetActor);
	check(Ability);

	// Set the target data actor's player controller.
	TargetActor->PrimaryPC = Ability->GetCurrentActorInfo()->PlayerController.Get();

	/* Register this task to callbacks for when the targeting data is ready. Note that we are registering the from the
	 * target actor to this task, not to the ASC. That is done in @RegisterTargetDataCallbacks. */
	TargetActor->TargetDataReadyDelegate.AddUObject(this, &UAbilityTask_HeroesWaitTargetData::OnTargetDataReady);
	TargetActor->CanceledDelegate.AddUObject(this, &UAbilityTask_HeroesWaitTargetData::OnTargetDataCancelled);
}

void UAbilityTask_HeroesWaitTargetData::FinalizeTargetActorInitialization() const
{
	check(TargetActor);
	check(Ability);

	TargetActor->StartTargeting(Ability);

	// Set up the target actor's confirmation logic.
	if (TargetActor->ShouldProduceTargetData())
	{
		switch (ConfirmationType)
		{
			// If the targeting should be instantly confirmed (e.g. a bullet firing), confirm it.
			case EGameplayTargetingConfirmation::Instant:
			{
				TargetActor->ConfirmTargeting();
				break;
			}
			// If the targeting should be confirmed by the player, register to when the player confirms or cancels it.
			case EGameplayTargetingConfirmation::UserConfirmed:
			{
				TargetActor->BindToConfirmCancelInputs();
				break;
			}
			default:
			{
			}
		}
	}
}

void UAbilityTask_HeroesWaitTargetData::OnTargetDataReady(const FGameplayAbilityTargetDataHandle& Data)
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!ASC || !Ability)
	{
		return;
	}

	FScopedPredictionWindow ScopedPrediction(ASC, ShouldReplicateDataToServer() && (bCreateKeyIfNotValidForMorePredicting && !ASC->ScopedPredictionKey.IsValidForMorePrediction()));

	// Replicate the target data to the server, while predicting it locally.
	const FGameplayAbilityActorInfo* Info = Ability->GetCurrentActorInfo();
	if (IsPredictingClient())
	{
		// Don't send target data from a client to the server if this target data should be exclusively generated by the server itself.
		if (!TargetActor->ShouldProduceTargetDataOnServer)
		{
			const FGameplayTag ApplicationTag;
			ASC->CallServerSetReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey(), Data, ApplicationTag, ASC->ScopedPredictionKey);
		}
		// If the user manually confirmed targeting, send an event to the server indicating that the user confirmed this targeting.
		else if (ConfirmationType == EGameplayTargetingConfirmation::UserConfirmed)
		{
			ASC->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GenericConfirm, GetAbilitySpecHandle(), GetActivationPredictionKey(), ASC->ScopedPredictionKey);
		}
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(Data);
	}

	// Only end this task if it should be ended when data is sent. Targeting abilities with custom confirmation handle ending the task on their own. 
	if (ConfirmationType != EGameplayTargetingConfirmation::Custom && ConfirmationType != EGameplayTargetingConfirmation::CustomMulti)
	{
		EndTask();
	}
}

void UAbilityTask_HeroesWaitTargetData::OnTargetDataCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!ASC)
	{
		return;
	}

	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get(), IsPredictingClient());

	// Tell the server that targeting was cancelled.
	if (IsPredictingClient())
	{
		if (!TargetActor->ShouldProduceTargetDataOnServer)
		{
			AbilitySystemComponent->ServerSetReplicatedTargetDataCancelled(GetAbilitySpecHandle(), GetActivationPredictionKey(), ASC->ScopedPredictionKey);
		}
		else
		{
			ASC->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GenericCancel, GetAbilitySpecHandle(), GetActivationPredictionKey(), ASC->ScopedPredictionKey);
		}
	}

	Cancelled.Broadcast(Data);
	EndTask();
}

void UAbilityTask_HeroesWaitTargetData::OnTargetDataReplicated(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag)
{
	// Valid target data that was replicated to the server for it to use.
	FGameplayAbilityTargetDataHandle MutableData = Data;

	if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
	{
		ASC->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	}

	/** 
	 *  Call into the TargetActor to sanitize/verify the data. If this returns false, we are rejecting
	 *	the replicated target data and will treat this as a cancel.
	 *	
	 *	This can also be used for bandwidth optimizations. OnReplicatedTargetDataReceived could do an actual
	 *	trace/check/whatever server side and use that data. So rather than having the client send that data
	 *	explicitly, the client is basically just sending a 'confirm' and the server is now going to do the work
	 *	in OnReplicatedTargetDataReceived.
	 */
	if (TargetActor && !TargetActor->OnReplicatedTargetDataReceived(MutableData))
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			Cancelled.Broadcast(MutableData);
		}
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			ValidData.Broadcast(MutableData);
		}
	}

	if (ConfirmationType != EGameplayTargetingConfirmation::CustomMulti)
	{
		EndTask();
	}
}

void UAbilityTask_HeroesWaitTargetData::OnTargetDataReplicatedCancelled()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		Cancelled.Broadcast(FGameplayAbilityTargetDataHandle());
	}

	// End the task on the server when told that the client cancelled targeting.
	EndTask();
}
