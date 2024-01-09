// Copyright Samuel Reitich 2024.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "HeroesAssetManager.generated.h"

class UHeroesGameData;
class UPrimaryDataAsset;

/**
 * Game implementation of the asset manager that handles game-specific loading logic.
 */
UCLASS(Config = Game)
class HEROESPROTOTYPEBASE_API UHeroesAssetManager : public UAssetManager
{
	GENERATED_BODY()

	// Asset manager.

public:

	/** Default constructor. */
	UHeroesAssetManager();

	/** Static getter for this singleton object. */
	static UHeroesAssetManager& Get();

protected:

	/** Runs game-specific loading logic. */
	virtual void StartInitialLoading() override;



	// Utils.

public:

	/** Synchronously loads the asset at the given path. */
	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);

	/** Thread-safe way of adding a loaded asset to keep in memory. */
	void AddLoadedAsset(const UObject* Asset);

	/** Returns the subclass referenced by a TSoftClassPtr.  This will synchronously load the asset if it's not already loaded. */
	template<typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);



	// Global game data.

public:

	/** Getter for the global game data. */
	const UHeroesGameData& GetGameData();

protected:

	/** Loads a global game data asset at the given path. */
	template <typename GameDataClass>
	const GameDataClass& GetOrLoadTypedGameData(const TSoftObjectPtr<GameDataClass>& DataPath)
	{
		if (TObjectPtr<UPrimaryDataAsset> const * pResult = GameDataMap.Find(GameDataClass::StaticClass()))
		{
			return *CastChecked<GameDataClass>(*pResult);
		}

		// Does a blocking load if needed.
		return *CastChecked<const GameDataClass>(LoadGameDataOfClass(GameDataClass::StaticClass(), DataPath, GameDataClass::StaticClass()->GetFName()));
	}

	/** Loads the given global game data into GameDataMap. */
	UPrimaryDataAsset* LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType);

	/** The global game data asset to use. This must be specified in DefaultGame.ini. */
	UPROPERTY(Config)
	TSoftObjectPtr<UHeroesGameData> GameDataPath;

	/** Loaded global game data. */
	UPROPERTY(Transient)
	TMap<TObjectPtr<UClass>, TObjectPtr<UPrimaryDataAsset>> GameDataMap;



	// Loaded assets.

private:
	
	/** Assets loaded and tracked by the asset manager. */
	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	/** Used for a scope lock when modifying the list of load assets. */
	FCriticalSection LoadedAssetsCritical;

};

template <typename AssetType>
TSubclassOf<AssetType> UHeroesAssetManager::GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	TSubclassOf<AssetType> LoadedSubclass;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedSubclass = AssetPointer.Get();
		if (!LoadedSubclass)
		{
			LoadedSubclass = Cast<UClass>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedSubclass, TEXT("Failed to load asset class [%s]"), *AssetPointer.ToString());
		}

		if (LoadedSubclass && bKeepInMemory)
		{
			// Added to loaded asset list.
			Get().AddLoadedAsset(Cast<UObject>(LoadedSubclass));
		}
	}

	return LoadedSubclass;
}