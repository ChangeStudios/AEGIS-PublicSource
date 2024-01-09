// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HealingExecutionDataAsset.generated.h"

/**
 * Data asset that defines rules for a gameplay effect's applied healing. These are used in the HealingExecution to
 * calculate the final applied healing value.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API UHealingExecutionDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	/** Whether or not this healing effect can affect the actor that created it. */
	UPROPERTY(EditDefaultsOnly, Category = "Healing Execution Data")
	bool bCanHealSelf = false;

	/** Whether or not this healing effect can affect enemies, or if it can only heal allies. */
	UPROPERTY(EditDefaultsOnly, Category = "Healing Execution Data")
	bool bCanHealEnemies = false;

	/** If this healing effect applies overhealth. If true, this effect will heal the target; if the target reaches
	 * their maximum health and there is leftover healing, it will be added to their overhealth. */
	UPROPERTY(EditDefaultsOnly, Category = "Healing Execution Data")
	bool bOverheal = false;

	/** If true, the healing value will be applied exactly, ignoring any incoming/outgoing healing multipliers. */
	UPROPERTY(EditDefaultsOnly, Category = "Healing Execution Data")
	bool bTrueHealing = false;

};
