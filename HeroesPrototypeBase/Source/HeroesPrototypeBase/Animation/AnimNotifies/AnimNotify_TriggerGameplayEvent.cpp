// Copyright Samuel Reitich 2023.


#include "Animation/AnimNotifies/AnimNotify_TriggerGameplayEvent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "HeroesLogChannels.h"

UAnimNotify_TriggerGameplayEvent::UAnimNotify_TriggerGameplayEvent()
	: Super()
{
	// Set this notify's default color in the editor.
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(0, 127, 0, 255);
#endif
}

FString UAnimNotify_TriggerGameplayEvent::GetNotifyName_Implementation() const
{
	// Return the name of the event tag used by this notify, if it's been set.
	if (EventTag.IsValid())
	{
		return EventTag.ToString();
	}

	// If the event tag hasn't been set, return the notify's default name.
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_TriggerGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	// Test MeshComp->GetOwner for valid ASC
	// Test validity of EventTag

	// Ensure the executing skeletal mesh component is a valid subobject and has a valid owner.
	if (!IsValid(MeshComp) || !IsValid(MeshComp->GetOwner()))
	{
		UE_LOG(LogHeroes, Warning, TEXT("UAnimNotify_TriggerGameplayEvent: Tried to trigger a gameplay event in animation [%s], but the skeletal mesh component and/or its owning actor were not valid."), *Animation->GetName());
		return;
	}

	// Ensure the executing skeletal mesh component's owner has an ASC.
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner());
	if (!IsValid(ASC))
	{
		UE_LOG(LogHeroes, Warning, TEXT("UAnimNotify_TriggerGameplayEvent: Tried to trigger a gameplay event in animation [%s], but the owning actor, [%s], does not have a valid ASC."), *Animation->GetName(), *MeshComp->GetOwner()->GetName());
		return;
	}

	// Ensure the given gameplay tag for the event is valid.
	if (!EventTag.IsValid())
	{
		UE_LOG(LogHeroes, Warning, TEXT("UAnimNotify_TriggerGameplayEvent: Tried to trigger a gameplay event in animation [%s] for [%s], but the gameplay event tag is not set or is not valid."), *Animation->GetName(), *MeshComp->GetOwner()->GetName());
		return;
	}

	// Package the data needed to send the gameplay event.
	FGameplayEventData EventData = FGameplayEventData();
	EventData.Instigator = MeshComp->GetOwner();
	EventData.EventTag = EventTag;

	// Send the gameplay event to the owner of the skeletal mesh component that triggered this notify.
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, EventData);

	Super::Notify(MeshComp, Animation, EventReference);
}
