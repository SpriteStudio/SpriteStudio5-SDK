#include "SpriteStudio5EdPrivatePCH.h"
#include "ReimportSspjFactory.h"
#include "SsProject.h"


#if defined(SS_UE4_4) || defined(SS_UE4_5)
UReimportSspjFactory::UReimportSspjFactory(const FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
#else
UReimportSspjFactory::UReimportSspjFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
#endif
{
}


bool UReimportSspjFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	USsProject* SsProject = Cast<USsProject>(Obj);
	if(SsProject && SsProject->AssetImportData)
	{
		OutFilenames.Add(FReimportManager::ResolveImportFilename(SsProject->AssetImportData->SourceFilePath, SsProject));
		return true;
	}
	return false;
}

void UReimportSspjFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	USsProject* SsProject = Cast<USsProject>(Obj);
	if(SsProject && ensure(NewReimportPaths.Num() == 1))
	{
		SsProject->AssetImportData->SourceFilePath = FReimportManager::ResolveImportFilename(NewReimportPaths[0], SsProject);
	}
}

EReimportResult::Type UReimportSspjFactory::Reimport(UObject* Obj)
{
	USsProject* SsProject = Cast<USsProject>(Obj);
	if(!SsProject)
	{
		return EReimportResult::Failed;
	}

	ExistImages.Empty();
	for(int i = 0; i < SsProject->CellmapList.Num(); ++i)
	{
		if(SsProject->CellmapList[i].Texture)
		{
			ExistImages.Add(SsProject->CellmapList[i].ImagePath, SsProject->CellmapList[i].Texture);
		}
	}

	const FString Filename = FReimportManager::ResolveImportFilename(SsProject->AssetImportData->SourceFilePath, SsProject);
	if(!Filename.Len() || IFileManager::Get().FileSize(*Filename) == INDEX_NONE)
	{
		return EReimportResult::Failed;
	}

	EReimportResult::Type Result = EReimportResult::Failed;
	if(UFactory::StaticImportObject(SsProject->GetClass(), SsProject->GetOuter(), *SsProject->GetName(), RF_Public | RF_Standalone, *Filename, NULL, this))
	{
		// Try to find the outer package so we can dirty it up
		if (SsProject->GetOuter())
		{
			SsProject->GetOuter()->MarkPackageDirty();
		}
		else
		{
			SsProject->MarkPackageDirty();
		}
		Result = EReimportResult::Succeeded;
	}
	else
	{
		Result = EReimportResult::Failed;
	}

	return Result;
}
