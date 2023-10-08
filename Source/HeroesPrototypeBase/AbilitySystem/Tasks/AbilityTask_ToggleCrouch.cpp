// Copyright Samuel Reitich 2023.


#include "AbilitySystem/Tasks/AbilityTask_ToggleCrouch.h"

#include "Components/TimelineComponent.h"
#include "Net/UnrealNetwork.h"

UAbilityTask_ToggleCrouch* UAbilityTask_ToggleCrouch::ToggleCrouch(UGameplayAbility* OwningAbility, FName TaskInstanceName, bool bStartCrouch)
{
	// Instantiate a new task.
	UAbilityTask_ToggleCrouch* MyTask = NewAbilityTask<UAbilityTask_ToggleCrouch>(OwningAbility, TaskInstanceName);

	// Set the new task's variables with the given parameters.
	MyTask->bStartCrouch = bStartCrouch;
	
	return MyTask;
}

void UAbilityTask_ToggleCrouch::Activate()
{
	
}

void UAbilityTask_ToggleCrouch::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	// Tick the crouching timeline.
	CrouchTimeline->TickTimeline(DeltaTime);
}

void UAbilityTask_ToggleCrouch::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAbilityTask_ToggleCrouch, bStartCrouch);
}