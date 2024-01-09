// Copyright Samuel Reitich 2024.


#include "Input/Components/NativeInputComponent.h"

#include "AbilitySystem/HeroesNativeGameplayTags.h"
#include "GameFramework/Pawn.h"
#include "HeroesLogChannels.h"
#include "Input/HeroesInputActionSet.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_InputTag_Look_Mouse, "InputTag.Look.Mouse", "Input tag to activate the look action with a mouse.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_InputTag_Look_Stick, "InputTag.Look.Stick", "Input tag to activate the look action with a gamepad.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_InputTag_Move, "InputTag.Move", "Input tag to activate the move action.");

UNativeInputComponent* UNativeInputComponent::FindNativeInputComponent(const AActor* Actor)
{
	// Try to find a component matching this class in the given actor. Return a nullptr otherwise.
	 return (Actor ? Actor->FindComponentByClass<UNativeInputComponent>() : nullptr);
}

void UNativeInputComponent::BindNativeActions(const UHeroesInputActionSet* ActionsToBind)
{
	TryBindNativeActionFromActionSet(ActionsToBind, TAG_InputTag_Look_Mouse, ETriggerEvent::Triggered, &ThisClass::Input_Look_Mouse);
	TryBindNativeActionFromActionSet(ActionsToBind, TAG_InputTag_Look_Stick, ETriggerEvent::Triggered, &ThisClass::Input_Look_Stick);
	TryBindNativeActionFromActionSet(ActionsToBind, TAG_InputTag_Move, ETriggerEvent::Triggered, &ThisClass::Input_Move);
}

template <typename FuncType>
void UNativeInputComponent::TryBindNativeActionFromActionSet(const UHeroesInputActionSet* SetToBindFrom, const FGameplayTag& NativeTagToFindInSet, ETriggerEvent TriggerEvent, FuncType Func, bool bLogIfNotFound)
{
	// Ensure the native tag we're trying to bind exists.
	if (!NativeTagToFindInSet.IsValid())
	{
		UE_LOG(LogHeroes, Error, TEXT("A native input component on actor %s tried to bind a native input tag that does not exist."), *GetNameSafe(GetOwner()));
		return;
	}

	/* If the given ability set has the specified tag, use the set to find the tag's input action and bind the specified
	 * handler function to that input action. */
	if (const UInputAction* InputActionToBind = SetToBindFrom->FindNativeInputActionForTag(NativeTagToFindInSet))
	{
		BindAction(InputActionToBind, TriggerEvent, this, Func);
	}
	// Print a warning if the given ability set does not have a definition for the specified tag.
	else if (bLogIfNotFound)
	{
		UE_LOG(LogHeroes, Warning, TEXT("Native input component on actor %s failed to bind an action to the tag %s. The ability set %s does not have a definition for the specified tag."), *GetNameSafe(GetOwner()), *NativeTagToFindInSet.GetTagName().ToString(), *GetNameSafe(SetToBindFrom));
	}
}

void UNativeInputComponent::Input_Look_Mouse(const FInputActionValue& InputActionValue)
{
	// Get the pawn that owns this component to control their input.
	APawn* Pawn = GetOwner<APawn>();

	if (!Pawn)
	{
		return;
	}

	// Apply the input action value to the owning pawn's controller.
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}

void UNativeInputComponent::Input_Look_Stick(const FInputActionValue& InputActionValue)
{
	// Get the pawn that owns this component to control their input.
	APawn* Pawn = GetOwner<APawn>();

	if (!Pawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();

	// Get the world to check how long the stick has been held.
	const UWorld* World = GetWorld();
	check(World);


	/* Scale the input action value based on the look-rate and the time the stick has been held before applying it to
	 * the pawn's controller. */
	if (Value.X != 0.0f)
	{
		// TODO: Add look-rate multiplier
		Pawn->AddControllerYawInput(Value.X * World->GetDeltaSeconds());
	}

	if (Value.Y != 0.0f)
	{
		// TODO: Add look-rate multiplier
		Pawn->AddControllerPitchInput(Value.Y * World->GetDeltaSeconds());
	}
}

void UNativeInputComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	// Get the pawn that owns this component and its controller to apply movement input.
	APawn* Pawn = GetOwner<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		// Add movement input using the pawn's built-in method.
		if (Value.X != 0.0f)
		{
			// Get the world axis to add movement input to.
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}
