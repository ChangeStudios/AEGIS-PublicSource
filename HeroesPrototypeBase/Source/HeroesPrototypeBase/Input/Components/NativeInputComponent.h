// Copyright Samuel Reitich 2023.

#pragma once

#include "InputActionValue.h"
#include "NativeGameplayTags.h"

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Input/HeroesInputActionSet.h"
#include "NativeInputComponent.generated.h"

/** Native gameplay tags relevant to this class. */
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputTag_Look_Mouse);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputTag_Look_Stick);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputTag_Move);

class UHeroesInputActionSet;

/**
 * An input component responsible for handling native input and input actions that are not a part of the GAS framework.
 * Input actions are manually bound to handler functions defined here using native gameplay tags.
 */
UCLASS(Config = Input)
class HEROESPROTOTYPEBASE_API UNativeInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:

	/** Returns the specified actor's NativeInputComponent if it has one. Returns a nullptr otherwise. */
	UFUNCTION(BlueprintPure, Category = "Heroes|Input|InputComponents")
	static UNativeInputComponent* FindNativeInputComponent(const AActor* Actor);

	/** Takes an action set and binds the actions in its native input action array to their corresponding handler
	 * functions in this class, if they exist. This uses each given input action's associated tag to search for a
	 * corresponding handler function. */
	void BindNativeActions(const UHeroesInputActionSet* ActionsToBind);

protected:

	/**
	 * Checks if the given ability set contains an action definition for the specified tag. If it does, it will get the
	 * input action that the ability set associates with that tag and bind the specified handler function to that input
	 * action.
	 *
	 * @param SetToBindFrom			The ability set used to bind the given tag. This ability set provides the input
	 *								action with which to bind the specified handler function.
	 * @param NativeTagToFindInSet	The gameplay tag used to retrieve the input action to bind to. This provides a way
	 *								to link an input action and the specified handler function. If the given ability set
	 *								does not have a definition for this tag, then this function does not bind anything.
	 * @param TriggerEvent			The trigger type to use for this binding. This is action-specific.
	 * @param Func					The handler function to which this action should be bound.
	 * @param bLogIfNotFound		Prints a debug warning if the given ability set does not have a definition for the
	 *								specified gameplay tag. False by default because ability sets may be intentionally
	 *								missing native input definitions.
	 */
	template<typename FuncType>
	void TryBindNativeActionFromActionSet(const UHeroesInputActionSet* SetToBindFrom, const FGameplayTag& NativeTagToFindInSet, ETriggerEvent TriggerEvent, FuncType Func, bool bLogIfNotFound = true);



	// Handler functions that are bound to native input actions.

protected:

	/** Handler function for looking input with a mouse. */
	void Input_Look_Mouse(const FInputActionValue& InputActionValue);

	/** Handler function for looking input with a gamepad stick. */
	void Input_Look_Stick(const FInputActionValue& InputActionValue);

	/** Handler function for movement input. */
	void Input_Move(const FInputActionValue& InputActionValue);

};
