// Copyright Samuel Reitich 2023.

#include "HeroesNativeGameplayTags.h"

#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "HeroesLogChannels.h"

FHeroesNativeGameplayTags FHeroesNativeGameplayTags::GameplayTags;

void FHeroesNativeGameplayTags::InitializeNativeTags()
{
	// Get the gameplay tag manager to handle adding tags.
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

	// Manually add every native gameplay tag with the gameplay tag manager.
	GameplayTags.AddAllTags(Manager);

	// We have to notify the gameplay tag manager that we are done adding gameplay tags.
	Manager.DoneAddingNativeTags();
}

FGameplayTag FHeroesNativeGameplayTags::FindTagByString(FString TagName, bool bMatchPartialName)
{
	// Try to find an exact match for the specified gameplay tag with the tag manager.
	const UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
	FGameplayTag ReturnTag = Manager.RequestGameplayTag(FName(*TagName), false);

	// If a tag could not be found and a partially matching tag can be accepted, try to find a partially matching tag.
	if (!ReturnTag.IsValid() && bMatchPartialName)
	{
		// Get every gameplay tag.
		FGameplayTagContainer AllTags;
		Manager.RequestAllGameplayTags(AllTags, true);

		// Iterate through every gameplay tag for a tag that has the given tag somewhere in it.
		for (const FGameplayTag PartialTag : AllTags)
		{
			if (PartialTag.ToString().Contains(TagName))
			{
				UE_LOG(LogHeroes, Warning, TEXT("Could not find an exact match for tag %s. Used partial match: %s."), *TagName, *PartialTag.ToString());
				ReturnTag = PartialTag;
				break;
			}
		}
	}

	return ReturnTag;
}

void FHeroesNativeGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	// Manually add every native input tag declared in this class.
	AddTag(Manager, SetByCaller_Damage, "SetByCaller.Damage", "Data tag to set the magnitude of a Set by Caller damage execution.");
	AddTag(Manager, SetByCaller_Healing, "SetByCaller.Healing", "Data tag to set the magnitude of a Set by Caller healing execution.");
	AddTag(Manager, SetByCaller_Overhealth, "SetByCaller.Overhealth",  "Data tag to set the magnitude of a Set by Caller overhealth execution.");
	AddTag(Manager, SetByCaller_Duration, "SetByCaller.Duration",  "Data tag to set the magnitude of the duration of a gameplay effect.");

	AddTag(Manager, State_Movement_Airborne, "State.Movement.Airborne", "Tags that describe why a character is airborne when they are in the air.");
	AddTag(Manager, State_Movement_Crouching, "State.Movement.Crouching", "The character is currently crouching. This is also given to characters who queue the “crouch” action, allowing them to crouch when possible.");
	AddTag(Manager, State_Aiming, "State.AimedDownSights", "The target is currently aiming down the sights of their equipped weapon.");
}

void FHeroesNativeGameplayTags::AddTag(UGameplayTagsManager& Manager, FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagDescription)
{
	// Construct a new gameplay tag with the given data and add it to the specified gameplay tag manager.
	OutTag = Manager.AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native) ")) + FString(TagDescription));
}
