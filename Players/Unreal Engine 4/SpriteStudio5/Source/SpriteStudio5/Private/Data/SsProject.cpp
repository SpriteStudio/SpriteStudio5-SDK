#include "SpriteStudio5PrivatePCH.h"
#include "SsProject.h"

#include "SsAnimePack.h"
#include "SsCellMap.h"
#include "SsString_uty.h"


#if defined(SS_UE4_4) || defined(SS_UE4_5)
USsProject::USsProject(const FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
#else
USsProject::USsProject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
#endif
{
}

void USsProject::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	if(Ar.IsLoading() || Ar.IsSaving())
	{
		for(int32 i = 0; i < AnimeList.Num(); ++i)
		{
			AnimeList[i].Serialize(Ar);
		}
	}
}

int32 USsProject::FindAnimePackIndex(const FName& AnimePackName)
{
	for(int32 i = 0; i < AnimeList.Num(); ++i)
	{
		if(AnimePackName == AnimeList[i].AnimePackName)
		{
			return i;
		}
	}
	return -1;
}

int32 USsProject::FindCellMapIndex(const FName& CellmapName)
{
	for(int32 i = 0; i < CellmapList.Num(); ++i)
	{
		if(CellmapName == CellmapList[i].FileName)
		{
			return i;
		}
	}
	return -1;
}

bool USsProject::FindAnimationIndex(const FName& InAnimPackName, const FName& InAnimationName, int32& OutAnimPackIndex, int32& OutAnimationIndex)
{
	OutAnimPackIndex = FindAnimePackIndex(InAnimPackName);
	if(OutAnimPackIndex < 0){ return false; }

	OutAnimationIndex = AnimeList[OutAnimPackIndex].FindAnimationIndex(InAnimationName);
	if(OutAnimationIndex < 0){ return false; }

	return true;
}

FSsAnimation* USsProject::FindAnimation(int32 AnimPackIndex, int32 AnimationIndex)
{
	if(AnimPackIndex < AnimeList.Num())
	{
		if(AnimationIndex < AnimeList[AnimPackIndex].AnimeList.Num())
		{
			return &(AnimeList[AnimPackIndex].AnimeList[AnimationIndex]);
		}
	}
	return NULL;
}

FString USsProject::GetSsceBasepath() const
{
	return getFullPath(ProjFilepath, Settings.CellMapBaseDirectory);
}
FString USsProject::GetSsaeBasepath() const
{
	return getFullPath(ProjFilepath, Settings.AnimeBaseDirectory);
}
FString USsProject::GetImageBasepath() const
{
	return getFullPath(ProjFilepath, Settings.ImageBaseDirectory);
}

