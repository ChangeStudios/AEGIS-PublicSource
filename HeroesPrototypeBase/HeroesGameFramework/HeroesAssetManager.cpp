// Copyright Samuel Reitich 2023.


#include "HeroesGameFramework/HeroesAssetManager.h"

#include "AbilitySystem/HeroesNativeGameplayTags.h"
#include "HeroesGameData.h"
#include "HeroesLogChannels.h"
#include "Misc/ScopedSlowTask.h"

UHeroesAssetManager::UHeroesAssetManager()
{
}

UHeroesAssetManager& UHeroesAssetManager::Get()
{
	check(GEngine);

	// Ensure that this is being used as the engine's asset manager.
	if (UHeroesAssetManager* Manager = Cast<UHeroesAssetManager>(GEngine->AssetManager))
	{
		return *Manager;
	}

	UE_LOG(LogHeroes, Fatal, TEXT("Invalid AssetManagerClassName in DefaultEngine.ini. Use HeroesAssetManager or the engine default."));

	// Create a new asset manager, even though it won't be the one used by the engine. The fatal error above prevents this from being called.
	return *NewObject<UHeroesAssetManager>();
}

void UHeroesAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	// Initialize the native gameplay tags to be used by our assets.
	FHeroesNativeGameplayTags::InitializeNativeTags();
}

UObject* UHeroesAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("Synchronously loaded asset [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);

		if (UAssetManager::IsValid())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
		}

		// Use LoadObject if asset manager isn't ready yet.
		return AssetPath.TryLoad();
	}

	return nullptr;
}

void UHeroesAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		LoadedAssets.Add(Asset);
	}
}

const UHeroesGameData& UHeroesAssetManager::GetGameData()
{
	return GetOrLoadTypedGameData<UHeroesGameData>(GameDataPath);
}

UPrimaryDataAsset* UHeroesAssetManager::LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType)
{
	UPrimaryDataAsset* Asset = nullptr;

	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("Loading GameData Object"), STAT_GameData, STATGROUP_LoadTime);

	if (!DataClassPath.IsNull())
	{
		UE_LOG(LogHeroes, Log, TEXT("Loading GameData: %s ..."), *DataClassPath.ToString());

		SCOPE_LOG_TIME_IN_SECONDS(TEXT("    ... GameData loaded!"), nullptr);

		// This can be called recursively in the editor because it is called on demand from PostLoad so force a sync load for primary asset and async load the rest in that case.
		if (GIsEditor)
		{
			Asset = DataClassPath.LoadSynchronous();
			LoadPrimaryAssetsWithType(PrimaryAssetType);
		}
		else
		{
			const TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);
			if (Handle.IsValid())
			{
				Handle->WaitUntilComplete(0.0f, false);

				// This should always work. The global data asset should always be a primary data asset.
				Asset = Cast<UPrimaryDataAsset>(Handle->GetLoadedAsset());
			}
		}
	}

	if (Asset)
	{
		GameDataMap.Add(DataClass, Asset);
	}
	else
	{
		// It is not acceptable to fail to load any GameData asset. It will result in soft failures that are hard to diagnose.
		UE_LOG(LogHeroes, Fatal, TEXT("Failed to load GameData asset at %s. Type %s. This is not recoverable and likely means you do not have the correct data to run %s."), *DataClassPath.ToString(), *PrimaryAssetType.ToString(), FApp::GetProjectName());
	}

	return Asset;
}