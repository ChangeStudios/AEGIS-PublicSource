// Copyright Samuel Reitich 2024.


#include "Animation/AnimInstances/Characters/NPCThirdPersonAnimInstance.h"

void UNPCThirdPersonAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// Utilize multi-threading to update animations.
	bUseMultiThreadedAnimationUpdate = true;
}
