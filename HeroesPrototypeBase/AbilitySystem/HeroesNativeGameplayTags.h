// Copyright Samuel Reitich 2023.

#pragma once

#include "GameplayTagContainer.h"

class UGameplayTagsManager;

/**
 * Singleton containing native gameplay tags that need to be exposed to C++ and are not modularized into other classes.
 * It is preferred to retrieve tags from here instead of using the gameplay tag manager to avoid having to manually
 * update references whenever a gameplay tag's name or hierarchy is changed. Before adding a tag here, try using
 * UE_DECLARE_GAMEPLAY_TAG_EXTERN to add it inside a relevant class to improve modularity.
 */
struct FHeroesNativeGameplayTags
{

private:

	/** Singleton instance of this struct. */
	static FHeroesNativeGameplayTags GameplayTags;



	// Functions for managing gameplay tags.

public:

	/** Returns the singleton instance of this struct. */
	static const FHeroesNativeGameplayTags& Get() { return GameplayTags; }

	/** Loads the native tags into the framework. This MUST be called on game initialization to properly
	 * use these tags. */
	static void InitializeNativeTags();

	/**
	 *	Attempts to find a gameplay tag in the gameplay tag manager that matches the specified name.
	 *
	 *	@param TagName				The name of the gameplay tag to search for.
	 *	@param bMatchPartialName	If an exact matching tag could not be found and this is true, we will search for a
	 *								a tag that CONTAINS the given name. E.g. "Jump" may return "InputTag.Jump"
	 *
	 *	@return						The gameplay tag matching the specified name. Returns null if one could not be
	 *								found.
	 */
	static FGameplayTag FindTagByString(FString TagName, bool bMatchPartialName = false);

protected:

	/** Defines and adds every gameplay tag we want to directly access in C++. */
	void AddAllTags(UGameplayTagsManager& Manager);

	/** Adds the given gameplay tag to the specified gameplay tag manager. This should only be used as a helper
	 * function for AddAllTags. */
	void AddTag(UGameplayTagsManager& Manager, FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagDescription);



	// Native gameplay tags. Add tags here.
	
public:

	// Set by Caller tags.
	FGameplayTag SetByCaller_Damage;
	FGameplayTag SetByCaller_Healing;
	FGameplayTag SetByCaller_Overhealth;
	FGameplayTag SetByCaller_Duration;

	
	// State tags.
	FGameplayTag State_Movement_Airborne;
	FGameplayTag State_Movement_Crouching;
	FGameplayTag State_Aiming;
	

};