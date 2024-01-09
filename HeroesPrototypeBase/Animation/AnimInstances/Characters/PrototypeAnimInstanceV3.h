// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimInstance.h"
#include "PrototypeAnimInstanceV3.generated.h"

class UInventoryComponent;
class AHeroesGamePlayerStateBase;
class UInventoryItemInstance;
class AHeroBase;
class UAbilitySystemComponent;
class UCharacterAnimationData;

/*
 * The type of animation data currently being used by this character. This defines the extent to which the animation
 * blueprint will operate. For example, if the current animation data is less than 1 (i.e. not an item or a weapon),
 * then animations like "equip" and "inspect" will not be considered.
 */
UENUM(BlueprintType)
enum EAnimationDataType : uint8
{
	E_CharacterAnimationData = 0,
	E_ItemAnimationData = 1,
	E_WeaponAnimationData = 2
};

/**
 * 
 */
UCLASS()
class HEROESPROTOTYPEBASE_API UPrototypeAnimInstanceV3 : public UAnimInstance
{
	GENERATED_BODY()

	// Initialization.

public:

	UPrototypeAnimInstanceV3(const FObjectInitializer& ObjectInitializer);
	
	virtual void NativeInitializeAnimation() override;

	virtual void NativeBeginPlay() override;



	// Animation updating.

public:

	/** Called each frame to update runtime variables. Used in place of NativeUpdateAnimation to implement multi-
	 * threading in order to reduce bottle-necking when updating multiple animation instances in one frame. This can
	 * only call thread-safe functions */
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;



	// Animation data.

public:

	/** Updates the animation data set currently being used by this character. This is called to switch between
	 * different animation sets, such as when changing items or weapons. */
	UFUNCTION(BlueprintCallable, Category = "Heroes|Animation|Animation Data")
	void UpdateCharacterAnimationData(UCharacterAnimationData* NewAnimData);

	/** The animation set to return to when there is no character animation set to use. This should ideally never be
	 * used because the player should never have a completely empty inventory. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Heroes|Animation|Animation Data")
	TObjectPtr<UCharacterAnimationData> DefaultCharacterAnimationData = nullptr;

protected:

	/** The current animation set being used by this character. */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Heroes|Animation|Animation Data")
	TObjectPtr<UCharacterAnimationData> CharacterAnimationData;

	/** The type of animation set being used. Updated with CharacterAnimationData; used as a faster way to query
	 * CharacterAnimationData's class. */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Heroes|Animation|Animation Data")
	TEnumAsByte<EAnimationDataType> AnimDataType;



	// Item animation.

protected:

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Heroes|Animation|Item Animation")
	TObjectPtr<UInventoryItemInstance> EquippedItem = nullptr;



	// Character animation states.

protected:

	/** Determines whether this character is currently crouched or standing. Updated with the State_Movement_Crouching
	 * gameplay tag. */
	UPROPERTY(BlueprintReadOnly, Category = "Heroes|Animation|State Variables")
	bool bIsCrouched = false;

	/** Determines whether this character is aiming. Updated with the State_Aiming gameplay tag. */
	UPROPERTY(BlueprintReadOnly, Category = "Heroes|Animation|State Variables")
	bool bIsAiming = false;

/* Callbacks for when this character's animation states change. Called when their corresponding gameplay tags are added
 * or removed. */
protected:

	UFUNCTION()
	FORCEINLINE void OnCrouchedStateChanged(const FGameplayTag Callback, int32 NewCount) { bIsCrouched = NewCount > 0; }
	FDelegateHandle CrouchStateChangedDelegate;

	UFUNCTION()
	FORCEINLINE void OnAimingStateChanged(const FGameplayTag Callback, int32 NewCount) { bIsAiming = NewCount > 0; }
	FDelegateHandle AimingStateChangedDelegate;



	// Utilities.

public:

	/** This animation instance's owning pawn, cached for convenience. */
	UPROPERTY(BlueprintReadOnly, Category = "Heroes|Animation|Utilities")
	TObjectPtr<AHeroBase> OwningHero = nullptr;

	/** This animation instance's owning character's player state. */
	UPROPERTY(BlueprintReadOnly, Category = "Heroes|Animation|Utilities")
	TObjectPtr<AHeroesGamePlayerStateBase> OwningPS = nullptr;

	/** This animation instance's owning character's ACS, if they have one. */
	UPROPERTY(BlueprintReadOnly, Category = "Heroes|Animation|Utilities")
	TObjectPtr<UAbilitySystemComponent> OwningACS = nullptr;

	/** This animation instance's owning character's inventory, if they have one. */
	UPROPERTY(BlueprintReadOnly, Category = "Heroes|Animation|Utilities")
	TObjectPtr<UInventoryComponent> PlayerInventory;
};
