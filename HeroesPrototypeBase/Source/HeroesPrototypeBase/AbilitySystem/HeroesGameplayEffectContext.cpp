// Copyright Samuel Reitich 2023.


#include "AbilitySystem/HeroesGameplayEffectContext.h"

FHeroesGameplayEffectContext* FHeroesGameplayEffectContext::GetHeroesContextFromHandle(FGameplayEffectContextHandle Handle)
{
	// This effect's context as the base gameplay effect context class.
	FGameplayEffectContext* EffectContext = Handle.Get();

	// If the effect context is valid and is of the type FHeroesGameplayEffectContext, cast it to that type and return it.
	if (EffectContext != nullptr && EffectContext->GetScriptStruct()->IsChildOf(FHeroesGameplayEffectContext::StaticStruct()))
	{
		// We can't use a C++ cast here; I'm not sure why but it doesn't really matter. A C-style cast works.
		return (FHeroesGameplayEffectContext*)EffectContext;
	}

	// Return nullptr if the effect context does not exist or is of the wrong type.
	return nullptr;
}

bool FHeroesGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

	return true;
}
