// Copyright Samuel Reitich 2024.


#include "GameplayTagStack.h"

#include "HeroesLogChannels.h"

void FGameplayTagStackContainer::AddTags(FGameplayTag Tag, int32 Quantity)
{
	// Ensure the given tag is valid.
	if (!Tag.IsValid())
	{
		UE_LOG(LogHeroes, Error, TEXT("FGameplayTagStackContainer: Invalid gameplay tag passed to FGameplayTagStackContainer::AddTags."))
		return;
	}

	// Ignore requests to add less than 1 tag.
	if (Quantity > 0)
	{
		// Search the stack array for a stack of the given tag.
		for (FGameplayTagStack& Stack : Stacks)
		{
			// If this container has a stack of the given tag, add the given quantity to it and its corresponding map entry.
			if (Stack.Tag == Tag)
			{
				const int32 NewQuantity = Stack.Quantity + Quantity;
				Stack.Quantity = NewQuantity;
				MarkItemDirty(Stack);

				StackMap[Tag] = NewQuantity;

				return;
			}
		}

		// If this container doesn't have a stack of the given tag, create one.
		FGameplayTagStack& NewStack = Stacks.Emplace_GetRef(Tag, Quantity);
		MarkItemDirty(NewStack);

		StackMap.Add(Tag, Quantity);
	}
}

void FGameplayTagStackContainer::RemoveTags(FGameplayTag Tag, int32 Quantity)
{
	// Ensure the given tag is valid.
	if (!Tag.IsValid())
	{
		UE_LOG(LogHeroes, Error, TEXT("FGameplayTagStackContainer: Invalid gameplay tag passed to FGameplayTagStackContainer::RemoveTags."))
		return;
	}

	// Ignore requests to remove less than 1 tag.
	if (Quantity > 0)
	{
		// Search the stack array for a stack of the given tag.
		for (auto It = Stacks.CreateIterator(); It; ++It)
		{
			FGameplayTagStack& Stack = *It;
			if (Stack.Tag == Tag)
			{
				const int32 NewQuantity = Stack.Quantity - Quantity;
			
				// If all of the tags will be removed, remove the stack from the array and the map.
				if (NewQuantity < 1)
				{
					It.RemoveCurrent();
					MarkArrayDirty();

					StackMap.Remove(Tag);
				}
				// If there will still at least 1 tag in the stack, update the stack and the map.
				else
				{
					Stack.Quantity = NewQuantity;
					MarkItemDirty(Stack);

					StackMap[Tag] = NewQuantity;
				}
			
				return;
			}
		}
	}
}

int32 FGameplayTagStackContainer::GetTagCount(FGameplayTag Tag) const
{
	// Use the stack map for fast queries.
	if (StackMap.Contains(Tag))
	{
		return StackMap[Tag];
	}

	return 0;
}

bool FGameplayTagStackContainer::ContainsTag(FGameplayTag Tag) const
{
	// Use the stack map for fast queries.
	return StackMap.Contains(Tag);
}
