// Copyright Samuel Reitich 2023.


#include "Inventory/ItemTraits/EquippableItemTrait.h"

#include "AbilitySystem/HeroesAbilitySystemGlobals.h"
#include "AbilitySystem/Components/HeroesAbilitySystemComponent.h"
#include "Animation/AnimInstances/Characters/PrototypeAnimInstanceV3.h"
#include "Animation/CharacterAnimationData/ItemCharacterAnimationData.h"
#include "Characters/Components/FirstPersonSkeletalMeshComponent.h"
#include "Characters/Heroes/HeroBase.h"
#include "Inventory/InventoryItemInstance.h"
#include "Player/PlayerStates/Game/HeroesGamePlayerStateBase.h"

void UEquippableItemTrait::OnEquipped(UInventoryItemInstance* ItemToEquip)
{
	AHeroBase* EquippingHero = Cast<AHeroBase>(ItemToEquip->GetCurrentOwner()->GetPawn());

	FActorSpawnParameters SpawnParams = FActorSpawnParameters();
	SpawnParams.Owner = const_cast<AHeroesGamePlayerStateBase*>(ItemToEquip->GetCurrentOwner());
	SpawnParams.Instigator = EquippingHero;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FTransform SpawnTransform = FTransform();
	const FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);

	check(ActorToSpawnOnEquip);

	// Spawn the item's first-person actor.
	FirstPersonEquippedActor = GetWorld()->SpawnActor(ActorToSpawnOnEquip, &SpawnTransform, SpawnParams);
	FirstPersonEquippedActor->AttachToComponent(EquippingHero->GetFirstPersonMesh(), AttachRules, EquipmentAttachSocketNames[AttachmentSocket]);
	FirstPersonEquippedActor->SetActorRelativeTransform(AttachmentOffset);

	// Spawn the item's third-person actor.
	ThirdPersonEquippedActor = GetWorld()->SpawnActor(ActorToSpawnOnEquip, &SpawnTransform, SpawnParams);
	ThirdPersonEquippedActor->AttachToComponent(EquippingHero->GetThirdPersonMesh(), AttachRules, EquipmentAttachSocketNames[AttachmentSocket]);
	ThirdPersonEquippedActor->SetActorRelativeTransform(AttachmentOffset);


	// Grant each of this item's on-equipped ability sets.
	if (UHeroesAbilitySystemComponent* HeroesASC = UHeroesAbilitySystemGlobals::GetHeroesAbilitySystemComponentFromActor(EquippingHero))
	{
		for (const UHeroesAbilitySet* Set : GrantedAbilitySetsOnEquip)
		{
			Set->GiveToAbilitySystem(HeroesASC, &GrantedAbilitySetHandles.Add_GetRef(FHeroesAbilitySet_GrantedHandles()), ItemToEquip);
		}
	}

	// TODO: Replace this when creating the final class.
	if (UPrototypeAnimInstanceV3* FPPAnimInstance = Cast<UPrototypeAnimInstanceV3>(EquippingHero->GetFirstPersonMesh()->GetAnimInstance()))
	{
		FPPAnimInstance->UpdateCharacterAnimationData(CharacterAnimationData);

		if (CharacterAnimationData->EquipAnim_FPP)
		{
			FPPAnimInstance->Montage_Play(CharacterAnimationData->EquipAnim_FPP);
		}
	}

	// Call any item-specific equipment logic.
	B_OnEquipped(ItemToEquip);
}

void UEquippableItemTrait::OnUnequipped(UInventoryItemInstance* ItemToUnequip)
{
	// Destroy both item actors.
	FirstPersonEquippedActor->Destroy();
	ThirdPersonEquippedActor->Destroy();

	// Remove each of this item's on-equipped ability sets.
	const AHeroBase* UnequippingHero = ItemToUnequip->GetCurrentOwner()->GetPawn<AHeroBase>();
	if (UHeroesAbilitySystemComponent* HeroesASC = UHeroesAbilitySystemGlobals::GetHeroesAbilitySystemComponentFromActor(UnequippingHero))
	{
		for (FHeroesAbilitySet_GrantedHandles Set : GrantedAbilitySetHandles)
		{
			Set.RemoveFromAbilitySystem(HeroesASC);
		}
	}

	// Call any item-specific equipment logic.
	B_OnUnequipped(ItemToUnequip);
}
