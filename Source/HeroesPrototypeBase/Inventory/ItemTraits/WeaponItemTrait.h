// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Inventory/ItemTraits/InventoryItemTraitBase.h"
#include "WeaponItemTrait.generated.h"

class UCurveVector;
class UWeaponStaticDataAsset;

/**
 * Designates this item as a weapon. Defines static weapon-related data and stores runtime data specific to each
 * weapon. We could alternatively put all of the static data into a "weapon data" data asset and have an object pointer
 * to that asset here.
 */
UCLASS(DisplayName = "Weapon", BlueprintType)
class HEROESPROTOTYPEBASE_API UWeaponItemTrait : public UInventoryItemTraitBase
{
	GENERATED_BODY()

	// Static data.

public:

	/** Static data for this weapon stored as a data asset. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UWeaponStaticDataAsset> StaticData;



	// Runtime data.

public:

	/** Determines recoil curve position and accuracy over time. Increases with each shot, decreases over time when
	 * not firing. */
	UPROPERTY(BlueprintReadWrite)
	float CurrentWeaponHeat = 0.5;

	/** The weapon heat at the time of the last shot. */
	UPROPERTY(BlueprintReadWrite)
	float PreviousWeaponHeat;


	

	/** The time that has passed since the last shot was fired. Updates when another shot is fired. */
	float TimeSinceLastShot;

	/** Timer used to control a weapon's rate of fire. Weapons with a maximum fire-rate must wait a certain amount of
	 * time between shots. */
	FTimerHandle TimeBetweenShots;

	/** The timeline handling the weapon's current recoil. Uses the recoil curve from this weapon's static data. */
	FTimeline RecoilTimeline;

	/** The timeline handling the weapon's recoil recovery when it stops firing. */
	FTimeline RecoilRecoveryTimeline;

	/** The rotation to return to if recoil recovery is enabled. If the player's rotation after they stop firing is
	 * drastically different from this, their rotation will not be reset. */
	FRotator ControlRotationBeforeFiring;

	/** Whether recoil will recover after this shot(s). This is set to false if the player attempts to control their
	 * recoil. */
	bool bShouldRecoverRecoil = true;

};
