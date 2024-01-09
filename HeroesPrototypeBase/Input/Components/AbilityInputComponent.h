// Copyright Samuel Reitich 2024.

#pragma once

#include "GameplayTagContainer.h"

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "AbilityInputComponent.generated.h"

class UHeroesAbilitySet;
class UHeroesInputActionSet;

/**
 * An input component responsible for handling input actions for abilities. Actions bound via this component attempt to
 * activate their corresponding ability using its tag when triggered.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API UAbilityInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:

	/** Returns the specified actor's AbilityInputComponent if it has one. Returns a nullptr otherwise. */
	UFUNCTION(BlueprintPure, Category = "Heroes|Input|InputComponents")
	static UAbilityInputComponent* FindAbilityInputComponent(const AActor* Actor);

	/** Binds the given actions to activate the corresponding ability with their matching input tag. */
	void BindAbilityActions(const UHeroesInputActionSet* ActionsToBind);

protected:

	/** Handler function called when an ability input action is pressed. Used to activate abilities using their tag. */
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);

	/** Handler function called when an ability input action is released. */
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
};
