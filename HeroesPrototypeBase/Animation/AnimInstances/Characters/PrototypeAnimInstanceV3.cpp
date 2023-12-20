// Copyright Samuel Reitich 2023.


#include "Animation/AnimInstances/Characters/PrototypeAnimInstanceV3.h"

#include "AbilitySystem/HeroesNativeGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "Animation/CharacterAnimationData/CharacterAnimationData.h"
#include "Animation/CharacterAnimationData/WeaponCharacterAnimationData.h"
#include "Characters/Heroes/HeroBase.h"
#include "Net/UnrealNetwork.h"
#include "Player/PlayerStates/Game/HeroesGamePlayerStateBase.h"

UPrototypeAnimInstanceV3::UPrototypeAnimInstanceV3(const FObjectInitializer& ObjectInitializer)
{
	// Enable multi-threading to update animations.
	bUseMultiThreadedAnimationUpdate = true;
}

void UPrototypeAnimInstanceV3::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// // Cache important variables.
	// OwningHero = TryGetPawnOwner() ? Cast<AHeroBase>(TryGetPawnOwner()) : nullptr;
	// OwningACS = OwningHero && OwningHero->GetAbilitySystemComponent() ? OwningHero->GetAbilitySystemComponent() : nullptr;
	//
	// // Bind state variables to be updated when their corresponding gameplay tags are added or removed.
	// if (IsValid(OwningACS))
	// {
	// 	OwningACS->RegisterGameplayTagEvent(FHeroesNativeGameplayTags::Get().State_Movement_Crouching, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UPrototypeAnimInstanceV3::OnCrouchedStateChanged);
	// 	OwningACS->RegisterGameplayTagEvent(FHeroesNativeGameplayTags::Get().State_Aiming, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UPrototypeAnimInstanceV3::OnAimingStateChanged);
	// 	UE_LOG(LogTemp, Error, TEXT("A"));
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("B"));
	// }
	//
	// // Initialize the character animation data.
	// UpdateCharacterAnimationData(DefaultCharacterAnimationData);
}

void UPrototypeAnimInstanceV3::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	// Cache important variables.
	OwningHero = TryGetPawnOwner() ? Cast<AHeroBase>(TryGetPawnOwner()) : nullptr;
	OwningPS = OwningHero && OwningHero->GetPlayerState() ? Cast<AHeroesGamePlayerStateBase>(OwningHero->GetPlayerState()) : nullptr;
	OwningACS = OwningHero && OwningHero->GetAbilitySystemComponent() ? OwningHero->GetAbilitySystemComponent() : nullptr;
	PlayerInventory = OwningPS && OwningPS->GetInventoryComponent() ? OwningPS->GetInventoryComponent() : nullptr;

	// Bind state variables to be updated when their corresponding gameplay tags are added or removed.
	if (IsValid(OwningACS))
	{
		CrouchStateChangedDelegate = OwningACS->RegisterGameplayTagEvent(FHeroesNativeGameplayTags::Get().State_Movement_Crouching, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UPrototypeAnimInstanceV3::OnCrouchedStateChanged);
		AimingStateChangedDelegate = OwningACS->RegisterGameplayTagEvent(FHeroesNativeGameplayTags::Get().State_Aiming, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UPrototypeAnimInstanceV3::OnAimingStateChanged);
	}

	// Initialize the character animation data.
	UpdateCharacterAnimationData(DefaultCharacterAnimationData);
}

void UPrototypeAnimInstanceV3::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}

void UPrototypeAnimInstanceV3::UpdateCharacterAnimationData(UCharacterAnimationData* NewAnimData)
{
	if (IsValid(NewAnimData))
	{
		// Update the current animation data set.
		CharacterAnimationData = NewAnimData;

		// Update the type of animation data set now being used.
		if (NewAnimData->IsA(UWeaponCharacterAnimationData::StaticClass()))
		{
			AnimDataType = E_WeaponAnimationData;
		}
		else if (NewAnimData->IsA(UItemCharacterAnimationData::StaticClass()))
		{
			AnimDataType = E_ItemAnimationData;
		}
		else
		{
			AnimDataType = E_CharacterAnimationData;
		}
	}
}

void UPrototypeAnimInstanceV3::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPrototypeAnimInstanceV3, CharacterAnimationData);
	DOREPLIFETIME(UPrototypeAnimInstanceV3, AnimDataType);
}
