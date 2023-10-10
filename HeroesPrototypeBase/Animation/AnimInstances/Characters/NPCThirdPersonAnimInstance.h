// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NPCThirdPersonAnimInstance.generated.h"

/**
 * The base third-person animation instance for all non-playable characters. This class should only be subclassed in
 * blueprints, not in C++.
 */
UCLASS()
class HEROESPROTOTYPEBASE_API UNPCThirdPersonAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

private:

	/** Default initializer. */
	virtual void NativeInitializeAnimation() override;

};
