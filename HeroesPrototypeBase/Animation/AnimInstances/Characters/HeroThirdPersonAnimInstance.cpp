// Copyright Samuel Reitich 2024.


#include "Animation/AnimInstances/Characters/HeroThirdPersonAnimInstance.h"

void UHeroThirdPersonAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// Utilize multi-threading to update animations.
	bUseMultiThreadedAnimationUpdate = true;
}
