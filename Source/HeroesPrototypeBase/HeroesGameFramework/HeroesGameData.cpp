// Copyright Samuel Reitich 2023.


#include "HeroesGameFramework/HeroesGameData.h"

#include "HeroesAssetManager.h"

const UHeroesGameData& UHeroesGameData::Get()
{
	return UHeroesAssetManager::Get().GetGameData();
}
