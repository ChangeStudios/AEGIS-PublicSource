// Copyright Samuel Reitich 2024.

#pragma once

#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "GameplayTagStack.generated.h"

struct FGameplayTagStackContainer;
struct FNetDeltaSerializeInfo;

/**
 * A "stack" of gameplay tags. A stack represents an exact quantity of a single gameplay tag.
 */
USTRUCT(BlueprintType)
struct FGameplayTagStack : public FFastArraySerializerItem
{
	GENERATED_BODY()

	/** Default constructor. */
	FGameplayTagStack()
	{}

	/** Constructor initializing the tag that this stack represents and the quantity of that tag. */
	FGameplayTagStack(FGameplayTag InTag, int32 InQuantity)
		: Tag(InTag), Quantity(InQuantity)
	{}

private:

	friend FGameplayTagStackContainer;

	/** The tag represented by this tag stack. */
	UPROPERTY()
	FGameplayTag Tag;

	/** The quantity of tags in this stack. */
	UPROPERTY()
	int32 Quantity = 0;
};

/**
 * A replicated collection of gameplay tag stacks. Each gameplay tag can only have one stack in a stack container.
 */
USTRUCT(BlueprintType)
struct FGameplayTagStackContainer : public FFastArraySerializer
{
	GENERATED_BODY()

	// Construction.

public:

	/** Default constructor. */
	FGameplayTagStackContainer()
	{}



	// Fast array serializer implementation.

public:

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FGameplayTagStack, FGameplayTagStackContainer>(Stacks, DeltaParams, *this);
	}



	// Tag stack handling.

public:

	/** Adds the given quantity of tags to the stack of the specified tag. If this container does not have a stack of
	 * the specified tag, creates and adds a new stack with the given quantity of the given tag.
	 * 
	 * @param Tag		The tag to remove from.
	 * @param Quantity	The quantity of tags to remove. Must be greater than 0.
	 */
	void AddTags(FGameplayTag Tag, int32 Quantity);

	/** Removes the given quantity of tags from the stack of the specified tag. Does nothing if this container does not
	 * have a stack of the specified tag. If this would remove all tags from a stack, the stack is destroyed.
	 * 
	 * @param Tag		The tag to remove from.
	 * @param Quantity	The quantity of tags to remove. Must be greater than 0.
	 */
	void RemoveTags(FGameplayTag Tag, int32 Quantity);

	/** Returns the quantity of the stack of the specified tag. Returns 0 if this container does not contain a stack of
	 * that tag. */
	int32 GetTagCount(FGameplayTag Tag) const;

	/** Returns true if this container has a stack of the specified tag AND that stack has at least one tag. */
	bool ContainsTag(FGameplayTag Tag) const;

private:

	/** This container's collection of gameplay tag stacks. This is replicated automatically by the fast array
	 * serializer. */
	UPROPERTY()
	TArray<FGameplayTagStack> Stacks;

	/** A map that mirrors @Stacks. Use this instead of @Stacks when speed is prioritized, e.g. for queries. */
	TMap<FGameplayTag, int32> StackMap;
};

/**
 * Fast array serializer implementation.
 */
template<>
struct TStructOpsTypeTraits<FGameplayTagStackContainer> : public TStructOpsTypeTraitsBase2<FGameplayTagStackContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};