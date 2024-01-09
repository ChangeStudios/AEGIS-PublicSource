// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "HeroesGameplayEffectBase.generated.h"

/**
 * The base gameplay effect class for all gameplay effects. This class should be derived from and not used directly.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API UHeroesGameplayEffectBase : public UGameplayEffect
{
	GENERATED_BODY()

public:

	/** Getter for this gameplay effect's execution data array. */
	UFUNCTION(BlueprintPure, Category = "Heroes|GameplayEffects|Executions")
	const TArray<UDataAsset*>& GetExecutionData() const { return ExecutionData; }

private:

	/** An array of data assets utilized by this gameplay effect's executions, if it has any. If an execution requires
	 * data from the effect that triggered it, add the data to this array. The execution can search in this array for
	 * its class and access the object. */
	UPROPERTY(EditDefaultsOnly, Category = GameplayEffect, meta = (DisplayAfter = "Executions"))
	TArray<TObjectPtr<UDataAsset>> ExecutionData;

};
