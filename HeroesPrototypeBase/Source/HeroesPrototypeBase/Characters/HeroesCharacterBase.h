// Copyright Samuel Reitich 2023.

#pragma once

#include "AbilitySystemInterface.h"
#include "NativeGameplayTags.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HeroesCharacterBase.generated.h"

/** Native gameplay tags relevant to this class. */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Identifier_Action_Generic_Crouch);

/**
 * The base class for all playable (e.g. heroes) and non-playable (e.g. bots) characters. This class should be derived
 * from and not used directly.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API AHeroesCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:

	/** Default constructor. */
	AHeroesCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());



	// Movement.

public:

	/** Identical to the default implementation but allows the character to be crouching. If we try to jump while
	 * crouching, we'll cancel our crouch and perform the jump. All jump abilities should cancel crouch abilities. */
	virtual bool CanJumpInternal_Implementation() const override;

	/** Removes state tags reliant on the previous movement state and attempts to execute queued movement state logic. */
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	/** Kills the character if they fall out of the world, if they have a health component. */
	virtual void FellOutOfWorld(const UDamageType& dmgType) override;
};
