// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "HeroesGameplayStatics.generated.h"

/**
 * Static class with global utilities that are exposed to C++ and blueprints.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API UHeroesGameplayStatics : public UBlueprintFunctionLibrary
{
 GENERATED_BODY()

public:

 /** Checks whether a vector can reach a target unobstructed. If the target is a pawn, the vector must have line of
  * sight to its feet and torso, torso and head, or all three if it wants to reach the target. */
 UFUNCTION(BlueprintPure, Category = "Heroes|Utils")
 static bool CanReachTarget(UWorld* InWorld, FVector StartVector, AActor* Target, TArray<AActor*> ActorsToIgnore);
};
