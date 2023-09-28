// Copyright Samuel Reitich 2023.


#include "AbilitySystem/AttributeSets/MovementAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "HeroesLogChannels.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/Heroes/HeroBase.h"
#include "Net/UnrealNetwork.h"

UMovementAttributeSet::UMovementAttributeSet()
{
	/* Initialize the attributes and set their base values. These base values will be overridden when we initialize the
	 * movement component with an ASC in UHeroesCharacterMovementComponent. */
	InitMovementSpeed(0.0f);
	InitMovementAcceleration(0.0f);
	InitDirectionalControl(0.0f);
	InitGravityScale(0.0f);
	InitJumpStrength(0.0f);
}

void UMovementAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Clamp attributes when they are modified.
	ClampAttribute(Attribute, NewValue);
}

void UMovementAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	// Clamp attributes' base values when they are modified.
	ClampAttribute(Attribute, NewValue);
}

void UMovementAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	// Clamp the attribute's value depending on the attribute.
	if (Attribute == GetMovementSpeedAttribute())
	{
		// 2000 cm/s is arbitrarily the maximum possible walk speed. This requires a speed boost of approximately 200%.
		constexpr float MaxMovementSpeed = 2000.0f;
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxMovementSpeed);
	}
	else if (Attribute == GetMovementAccelerationAttribute())
	{
		/* 6000 cm/s^2 is arbitrarily the maximum acceleration speed. This requires an acceleration boost of
		 * approximately 300%. */
		constexpr float MinAccelerationSpeed = 0.0f;
		constexpr float MaxAccelerationSpeed = 6000.0f;
		NewValue = FMath::Clamp(NewValue, MinAccelerationSpeed, MaxAccelerationSpeed);
	}
	else if (Attribute == GetDirectionalControlAttribute())
	{
		/* 1.0 represents the default directional control values. 0.05 if an arbitrary minimum because we never want
		 * the player to have 0 directional control. */
		constexpr float MinDirectionalControl = 0.05f;
		NewValue = FMath::Clamp(NewValue, MinDirectionalControl, 1.0f);
	}
	else if (Attribute == GetGravityScaleAttribute())
	{
		/* Gravity can be set to 0.0 to disable it. 4.0 is an arbitrary maximum that results in an increase of
		 * approximately 200% from the game's base gravity scale. */
		constexpr float MaxGravityScale = 4.0f;
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxGravityScale);
	}
	else if (Attribute == GetJumpStrengthAttribute())
	{
		/* 650 and 7800 are arbitrary values for the minimum and maximum jump strength, respectively. With a gravity
		 * scale of 2.0, they result in jump heights of about 1.0 meter and 12.0 meters, respectively. The minimum is
		 * also the default jump strength because we should never be decreasing jump strength below its default value. */
		constexpr float MinJumpStrength = 650.0f;
		constexpr float MaxJumpStrength = 7800.0f;
		NewValue = FMath::Clamp(NewValue, MinJumpStrength, MaxJumpStrength);
	}
}

void UMovementAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldValue)
{
	// Broadcast a rep notify for the MovementSpeed attribute.
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMovementAttributeSet, MovementSpeed, OldValue);
}

void UMovementAttributeSet::OnRep_MovementAcceleration(const FGameplayAttributeData& OldValue)
{
	// Broadcast a rep notify for the MovementAcceleration attribute.
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMovementAttributeSet, MovementAcceleration, OldValue);
}

void UMovementAttributeSet::OnRep_DirectionalControl(const FGameplayAttributeData& OldValue)
{
	// Broadcast a rep notify for the DirectionalControl attribute.
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMovementAttributeSet, DirectionalControl, OldValue);
}

void UMovementAttributeSet::OnRep_GravityScale(const FGameplayAttributeData& OldValue)
{
	// Broadcast a rep notify for the GravityScale attribute.
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMovementAttributeSet, GravityScale, OldValue);
}

void UMovementAttributeSet::OnRep_JumpStrength(const FGameplayAttributeData& OldValue)
{
	// Broadcast a rep notify for the JumpStrength attribute.
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMovementAttributeSet, JumpStrength, OldValue);
}

void UMovementAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate our attribute values.
	DOREPLIFETIME_CONDITION_NOTIFY(UMovementAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMovementAttributeSet, MovementAcceleration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMovementAttributeSet, DirectionalControl, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMovementAttributeSet, GravityScale, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMovementAttributeSet, JumpStrength, COND_None, REPNOTIFY_Always);
}