// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BaseHealthAttributeValueData.generated.h"

/**
 * Data asset that defines the base values for health attributes.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API UBaseHealthAttributeValueData : public UDataAsset
{
	GENERATED_BODY()

public:

	/** The base value for the Health attribute. */
	UPROPERTY(EditDefaultsOnly, Category = "Base Health Attribute Values")
	float BaseHealth = 100.0f;

	/** The base value for the MaximumHealth attribute. */
	UPROPERTY(EditDefaultsOnly, Category = "Base Health Attribute Values")
	float BaseMaximumHealth = 100.0f;
	
	/** The base value for the Overhealth attribute. */
	UPROPERTY(EditDefaultsOnly, Category = "Base Health Attribute Values")
	float BaseOverhealth = 0.0f;
	
	/** The base value for the MaximumOverhealth attribute. */
	UPROPERTY(EditDefaultsOnly, Category = "Base Health Attribute Values")
	float BaseMaximumOverhealth = 100.0f;


	/** The base value for the Damage meta attribute. */
	UPROPERTY(EditDefaultsOnly, Category = "Base Health Meta Attribute Values")
	float BaseDamage = 0.0f;
	
	/** The base value for the Healing meta attribute. */
	UPROPERTY(EditDefaultsOnly, Category = "Base Health Meta Attribute Values")
	float BaseHealing = 0.0f;
	
	/** The base value for the Overhealing meta attribute. */
	UPROPERTY(EditDefaultsOnly, Category = "Base Health Meta Attribute Values")
	float BaseOverhealing = 0.0f;

};
