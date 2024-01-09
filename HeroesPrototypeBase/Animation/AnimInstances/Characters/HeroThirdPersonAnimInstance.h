// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HeroThirdPersonAnimInstance.generated.h"

/**
 * The base third-person animation instance for all playable characters ("heroes"). This class should only be
 * subclassed in blueprints, not in C++. This class is subclassed into a base animation blueprint, which can then be
 * subclassed into additional animation blueprints to add additional functionality on a per-hero basis, if needed.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API UHeroThirdPersonAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

private:

	/** Default initializer. */
	virtual void NativeInitializeAnimation() override;

};
