// Copyright Samuel Reitich 2024.


#include "HeroesGameFramework/HeroesGameData.h"

#include "HeroesAssetManager.h"

const UHeroesGameData& UHeroesGameData::Get()
{
	return UHeroesAssetManager::Get().GetGameData();
}
