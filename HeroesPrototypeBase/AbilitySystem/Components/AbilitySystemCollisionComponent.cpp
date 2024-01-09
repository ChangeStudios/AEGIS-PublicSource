// Copyright Samuel Reitich 2024.


#include "AbilitySystem/Components/AbilitySystemCollisionComponent.h"

#include "HeroesAbilitySystemComponent.h"
#include "AbilitySystem/HeroesAbilitySystemGlobals.h"
#include "Kismet2/ComponentEditorUtils.h"

UAbilitySystemCollisionComponent::UAbilitySystemCollisionComponent()
{
}

void UAbilitySystemCollisionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Bind the specified component to detect when another actor overlaps this actor.
	TArray<UActorComponent*> Components;
	GetOwner()->GetComponents(USceneComponent::StaticClass(), Components);

	for (UActorComponent* Component : Components)
	{
		if (Component->GetFName().IsEqual(CollisionComponent, ENameCase::IgnoreCase))
		{
			Cast<UPrimitiveComponent>(Component)->OnComponentBeginOverlap.AddDynamic(this, &UAbilitySystemCollisionComponent::OnActorBeginOverlap);
			Cast<UPrimitiveComponent>(Component)->OnComponentEndOverlap.AddDynamic(this, &UAbilitySystemCollisionComponent::OnActorEndOverlap);
		}
	}
}

void UAbilitySystemCollisionComponent::OnActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Only grant ability sets on the server.
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	// Grant the ability sets that are granted when an actor enters this actor's collision component.
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor))
	{
		for (auto Ability : PermanentAbilitiesToGive)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability));
		}

		EnteredActorsInfoMap.Add(OtherActor);

		for (auto Ability : OngoingAbilitiesToGive)
		{
			FGameplayAbilitySpecHandle AbilityHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability));

			EnteredActorsInfoMap[OtherActor].AppliedAbilities.Add(AbilityHandle);
		}

		for (auto GameplayEffect : PermanentEffectsToApply)
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();

			EffectContext.AddInstigator(OtherActor, OtherActor);

			FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
			if (SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}

		for (auto GameplayEffect : OngoingEffectsToApply)
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();

			EffectContext.AddInstigator(OtherActor, OtherActor);

			FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
			if (SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				if (ActiveGEHandle.WasSuccessfullyApplied())
				{
					EnteredActorsInfoMap[OtherActor].AppliedEffects.Add(ActiveGEHandle);
				}
			}
		}
	}
}

void UAbilitySystemCollisionComponent::OnActorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Only remove ability sets on the server.
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	// Remove the temporary ability sets that are removed when an actor leaves this actor's collision component.
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor))
	{
		if (EnteredActorsInfoMap.Find(OtherActor))
		{
			for (auto GameplayAbilityHandle : EnteredActorsInfoMap[OtherActor].AppliedAbilities)
			{
				AbilitySystemComponent->ClearAbility(GameplayAbilityHandle);
			}

			for (auto GameplayEffectHandle : EnteredActorsInfoMap[OtherActor].AppliedEffects)
			{
				AbilitySystemComponent->RemoveActiveGameplayEffect(GameplayEffectHandle);
			}

			EnteredActorsInfoMap.Remove(OtherActor);
		}
	}
}
