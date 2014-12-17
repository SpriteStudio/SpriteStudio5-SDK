#pragma once

#include "BaseToolkit.h"
#include "AssetEditorToolkit.h"

class FSsProjectViewer : public FAssetEditorToolkit, public FTickableEditorObject
{
public:
	static TSharedRef<FSsProjectViewer> CreateEditor( const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UObject* ObjectToEdit );

public:
	// IToolkit interface
	virtual void RegisterTabSpawners( const TSharedRef<class FTabManager>& TabManager ) override;
	virtual void UnregisterTabSpawners( const TSharedRef<class FTabManager>& TabManager ) override;

	// FAssetEditorTookkit interface
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;

	// FTickableObjectBase interface
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(FSsProjectViewer, STATGROUP_Tickables); }
	virtual bool IsTickable() const override { return true; }
	virtual void Tick(float DeltaTime) override;

private:
	void InitEditor( const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UObject* ObjectToEdit );

	TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);

	void ExtendToolbar();
	void BindCommands();

	// Toolbar
	void OnPlay();
	bool IsPlaying() const;
	void OnPrevFrame();
	void OnNextFrame();
	void OnAnimePackChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type);
	void OnAnimationChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type);
	void OnSetFrame(int32 Frame, ETextCommit::Type);
	TOptional<int32> GetNowFrame() const;
	void OnChangeDrawGrid();
	bool IsDrawGrid() const;
	void OnSetGridSize(int32 Size, ETextCommit::Type);
	TOptional<int32> GetGridSize() const;
	FReply OnGridColorMouseDown(const FGeometry&, const FPointerEvent& MouseEvent);
	void OnSetGridColor(FLinearColor Color);
	FLinearColor GetGridColor() const;
	FReply OnBackColorMouseDown(const FGeometry&, const FPointerEvent& MouseEvent);
	void OnSetBackColor(FLinearColor Color);
	FLinearColor GetBackColor() const;

private:
	TSharedPtr<class SSsProjectViewerViewport> Viewport;

	class USsProject* SsProject;
	struct FSsAnimePack* CurrentAnimePack;
	TSharedPtr<class FSsPlayer> Player;

	TArray<TSharedPtr<FString>> AnimePackNames;
	TArray<TSharedPtr<FString>> AnimationNames;
	TSharedPtr<class STextComboBox> AnimationCombo;
	TSharedPtr<class SColorBlock> GridColorBlock;
	TSharedPtr<class SColorBlock> BackColorBlock;
	TSharedPtr<class STextBlock>  MaxFrameText;
};
