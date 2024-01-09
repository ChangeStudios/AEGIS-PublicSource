// Copyright Samuel Reitich 2024.

#pragma once

#include "GameplayEffectTypes.h"

#include "HeroesGameplayEffectContext.generated.h"

/**
 * The struct for all gameplay effect contexts. This provides data about gameplay effects for executions to use. It can
 * be retrieved with a context handle with GetHeroesContextFromHandle.
 */
USTRUCT()
struct FHeroesGameplayEffectContext: public FGameplayEffectContext
{
 GENERATED_BODY()

public:

 /** Default constructor. */
 FHeroesGameplayEffectContext() : FGameplayEffectContext() {}

 /** Constructor providing an instigator (e.g. a player that's throwing a grenade) and an effect causer (e.g. the
  * grenade that's dealing damage). */
 FHeroesGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser) : FGameplayEffectContext(InInstigator, InEffectCauser) {}

 /** Constructor providing an instigator and an effect causer. */
 static HEROESPROTOTYPEBASE_API FHeroesGameplayEffectContext* GetHeroesContextFromHandle(FGameplayEffectContextHandle Handle);

 /** Override the function to retrieve this structure's static structure. */
 virtual UScriptStruct* GetScriptStruct() const override
 {
  return FHeroesGameplayEffectContext::StaticStruct();
 }

 /** Serializes new fields. */
 virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
};

template<>
struct TStructOpsTypeTraits<FHeroesGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FHeroesGameplayEffectContext>
{
 enum
 {
  WithNetSerializer = true
 };
};