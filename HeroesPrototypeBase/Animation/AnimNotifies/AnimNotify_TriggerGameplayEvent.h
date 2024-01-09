// Copyright Samuel Reitich 2024.

#pragma once

#include "GameplayTagContainer.h"

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_TriggerGameplayEvent.generated.h"

/**
 * An animation notify that triggers a gameplay event with a given gameplay event tag.
 */
UCLASS(Const, HideCategories = Object, CollapseCategories, meta = (DisplayName = "Trigger Gameplay Event"))
class HEROESPROTOTYPEBASE_API UAnimNotify_TriggerGameplayEvent : public UAnimNotify
{
	GENERATED_BODY()

	// Animation notify.

public:

	/** Default constructor. Sets the notify's default color in the editor. */
	UAnimNotify_TriggerGameplayEvent();

	/** Returns this notify's gameplay event tag as its name, if the tag is valid. */
	virtual FString GetNotifyName_Implementation() const override;

	/** Called when this notify is triggered. Sends a gameplay event to the owner of the skeletal mesh tha triggered
	 * this notify using the given gameplay event tag. */
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;



	// User-exposed parameters.

public:

	/** The gameplay tag used to trigger the gameplay event. The gameplay event is sent to the owner of the skeletal
	 * mesh component that triggered this notify. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (DisplayName = "Gameplay Event Tag"))
	FGameplayTag EventTag;

};
