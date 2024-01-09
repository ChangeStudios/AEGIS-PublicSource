// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HeroesPlayerStateBase.generated.h"

class UHeroesAbilitySystemComponent;

/**
 * The base class for all Heroes player states. This class should be derived from and not used directly.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API AHeroesPlayerStateBase : public APlayerState
{
	GENERATED_BODY()

};
