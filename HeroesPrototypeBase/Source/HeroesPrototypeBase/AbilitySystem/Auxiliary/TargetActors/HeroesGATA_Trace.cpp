// Copyright Samuel Reitich 2023.


#include "AbilitySystem/Auxiliary/TargetActors/HeroesGATA_Trace.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"

void AHeroesGATA_Trace::Configure(AActor* InSourceActor, FCollisionProfileName InTraceProfile, bool bInIgnoreBlockingHits, bool bInShouldProduceTargetDataOnServer, float InMaxRange)
{
	SourceActor = InSourceActor;
	TraceProfile = InTraceProfile;
	bIgnoreBlockingHits = bInIgnoreBlockingHits;
	bShouldProduceTargetDataOnServer = bInShouldProduceTargetDataOnServer;
	MaxRange = InMaxRange;
}

void AHeroesGATA_Trace::CancelTargeting()
{
	const FGameplayAbilityActorInfo* ActorInfo = (OwningAbility ? OwningAbility->GetCurrentActorInfo() : nullptr);
	UAbilitySystemComponent* ASC = (ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr);
	if (ASC)
	{
		ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GenericCancel, OwningAbility->GetCurrentAbilitySpecHandle(), OwningAbility->GetCurrentActivationInfo().GetActivationPredictionKey()).Remove(GenericCancelHandle);
	}

	CanceledDelegate.Broadcast(FGameplayAbilityTargetDataHandle());
}

void AHeroesGATA_Trace::ConfirmTargetingAndContinue()
{
	check(ShouldProduceTargetData());

	if (SourceActor)
	{
		TArray<FHitResult> HitResults = PerformTrace(SourceActor);
		FGameplayAbilityTargetDataHandle Data;

		for (int32 i = 0; i < HitResults.Num(); i++)
		{
			FGameplayAbilityTargetData_SingleTargetHit* ReturnData = new FGameplayAbilityTargetData_SingleTargetHit();
			ReturnData->HitResult = HitResults[i];
			Data.Add(ReturnData);
		}

		TargetDataReadyDelegate.Broadcast(Data);
	}
}

TArray<FHitResult> AHeroesGATA_Trace::PerformTrace(AActor* InSourceActor)
{
	bool bTraceComplex = false;
	TArray<AActor*> ActorsToIgnore;

	ActorsToIgnore.Add(InSourceActor);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(AHeroesGATA_Trace), bTraceComplex);
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActors(ActorsToIgnore);
	Params.bIgnoreBlocks = bIgnoreBlockingHits;

	check(PrimaryPC)

	FVector ViewStart;
	FRotator ViewRot;
	PrimaryPC->GetPlayerViewPoint(ViewStart, ViewRot);

	TArray<FHitResult> ReturnHitResults;

	const FVector TraceStart = ViewStart;
	const FVector ViewDir = ViewRot.Vector();
	const FVector TraceEnd = ViewStart + (ViewDir * MaxRange);

	TArray<FHitResult> HitResults;
	InSourceActor->GetWorld()->LineTraceMultiByProfile(HitResults, TraceStart, TraceEnd, TraceProfile.Name, Params);

	SetActorLocationAndRotation(TraceEnd, SourceActor->GetActorRotation());

	if (HitResults.Num() < 1)
	{
		FHitResult HitResult;

		HitResult.TraceStart = TraceStart;
		HitResult.TraceEnd = TraceEnd;
		HitResult.Location = TraceEnd;
		HitResult.ImpactPoint = TraceEnd;
		HitResults.Add(HitResult);
	}

	return HitResults;
}
