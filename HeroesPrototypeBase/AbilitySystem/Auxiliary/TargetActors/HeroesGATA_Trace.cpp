// Copyright Samuel Reitich 2023.


#include "AbilitySystem/Auxiliary/TargetActors/HeroesGATA_Trace.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "HeroesLogChannels.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/HeroesNativeGameplayTags.h"
#include "Curves/CurveVector.h"
#include "Inventory/ItemTraits/WeaponItemTrait.h"
#include "Inventory/ItemTraits/WeaponStaticDataAsset.h"

void AHeroesGATA_Trace::Configure(AActor* InSourceActor, FCollisionProfileName InTraceProfile, bool bInIgnoreBlockingHits, bool bInShouldProduceTargetDataOnServer, float InMaxRange, UWeaponItemTrait* InWeaponItemTrait)
{
	SourceActor = InSourceActor;
	TraceProfile = InTraceProfile;
	bIgnoreBlockingHits = bInIgnoreBlockingHits;
	bShouldProduceTargetDataOnServer = bInShouldProduceTargetDataOnServer;
	MaxRange = InMaxRange;
	WeaponItemTrait = InWeaponItemTrait;
	bDestroyOnConfirmation = false;
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
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(SourceActor);

	bool bTraceComplex = false;
	TArray<AActor*> ActorsToIgnore;

	ActorsToIgnore.Add(InSourceActor);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(AHeroesGATA_Trace), bTraceComplex);
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActors(ActorsToIgnore);
	Params.bIgnoreBlocks = bIgnoreBlockingHits;

	check(PrimaryPC);

	FVector ViewStart;
	FRotator ViewRot;
	PrimaryPC->GetPlayerViewPoint(ViewStart, ViewRot);

	TArray<FHitResult> ReturnHitResults;

	const FVector TraceStart = ViewStart;

	// Randomize the view rotation depending on accuracy
	FVector ViewDir = ViewRot.Vector();
	ensure(WeaponItemTrait);
	UWeaponStaticDataAsset* WeaponData = WeaponItemTrait->StaticData;
	float CurrentHeat = WeaponItemTrait->CurrentWeaponHeat;
	FVector Spread = WeaponData->SpreadCurve->GetVectorValue(CurrentHeat);
	Spread *= ASC && ASC->HasMatchingGameplayTag(FHeroesNativeGameplayTags::Get().State_AimedDownSights) ? WeaponData->AimingSpreadMultiplier : 1.0f;
	float SpreadX = FMath::DegreesToRadians(Spread.X);
	float SpreadY = FMath::DegreesToRadians(Spread.Y);
	const int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);
	const FVector ViewDirWithSpread = WeaponRandomStream.VRandCone(ViewDir, SpreadX, SpreadY);

	UE_LOG(LogHeroes, Warning, TEXT("CurrentHeat: %f, Spread: %f, %f"), CurrentHeat, SpreadX, SpreadY);
	
	const FVector TraceEnd = ViewStart + (ViewDirWithSpread * MaxRange);
	CurrentTraceEnd = TraceEnd;

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
