// Copyright Samuel Reitich 2023.


#include "Animation/AnimInstances/Characters/HeroThirdPersonAnimInstance.h"

void UHeroThirdPersonAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// Utilize multi-threading to update animations.
	bUseMultiThreadedAnimationUpdate = true;
}
