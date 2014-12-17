#include "SpriteStudio5EdPrivatePCH.h"

#include "AssetTypeActions_SsProject.h"

class FSpriteStudio5Ed : public ISpriteStudio5Ed
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	TSharedPtr<FAssetTypeActions_SsProject> SspjAssetTypeActions;
};

IMPLEMENT_MODULE(FSpriteStudio5Ed, SpriteStudio5Ed)



void FSpriteStudio5Ed::StartupModule()
{
	SspjAssetTypeActions = MakeShareable(new FAssetTypeActions_SsProject);
	FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get().RegisterAssetTypeActions(SspjAssetTypeActions.ToSharedRef());

	Style = MakeShareable(new FSpriteStudio5EdStyle());
}


void FSpriteStudio5Ed::ShutdownModule()
{
	if (SspjAssetTypeActions.IsValid())
	{
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get().UnregisterAssetTypeActions(SspjAssetTypeActions.ToSharedRef());
		}
		SspjAssetTypeActions.Reset();
	}
}



