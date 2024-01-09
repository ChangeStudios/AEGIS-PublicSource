// Copyright Samuel Reitich 2024.


#include "AbilitySystem/Abilities/Generic/GA_DropItem.h"

#include "HeroesLogChannels.h"
#include "Characters/Heroes/HeroBase.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/InventoryItemInstance.h"
#include "Player/PlayerStates/Game/HeroesGamePlayerStateBase.h"

UGA_DropItem::UGA_DropItem(const FObjectInitializer& ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	ActivationStyle = EHeroesAbilityActivationStyle::ActivateOnInputTriggered;
}

bool UGA_DropItem::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	// Ensure we have a valid avatar whose equipped item we can try to drop.
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	// We can only drop an item if the player currently has a valid item equipped.
	AHeroBase* Hero = Cast<AHeroBase>(ActorInfo->AvatarActor.Get());
	AHeroesGamePlayerStateBase* HeroPS = Hero ? Cast<AHeroesGamePlayerStateBase>(Hero->GetPlayerState()) : nullptr;
	if (HeroPS)
	{
		if (IsValid(HeroPS->GetInventoryComponent()->GetEquippedItem()))
		{
			return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
		}
	}

	return false;
}



void UGA_DropItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Try to drop the player's currently equipped item.
	AHeroBase* Hero = Cast<AHeroBase>(ActorInfo->AvatarActor.Get());
	AHeroesGamePlayerStateBase* HeroPS = Hero ? Cast<AHeroesGamePlayerStateBase>(Hero->GetPlayerState()) : nullptr;
	if (HeroPS)
	{
		UInventoryItemInstance* ItemToDrop = HeroPS->GetInventoryComponent()->GetEquippedItem();
		HeroPS->GetInventoryComponent()->TryDropItem(ItemToDrop, true);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
