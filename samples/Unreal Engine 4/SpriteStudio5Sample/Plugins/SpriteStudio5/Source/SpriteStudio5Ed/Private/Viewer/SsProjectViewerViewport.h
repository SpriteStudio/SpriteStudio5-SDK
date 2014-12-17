#pragma once

class SSsProjectViewerViewport : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSsProjectViewerViewport) {}
	SLATE_END_ARGS()

public:
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	void Construct(const FArguments& InArgs);

	void SetPlayer(TWeakPtr<class FSsPlayer> InPlayer);

private:
	TSharedPtr<FSceneViewport> Viewport;
	TSharedPtr<SViewport> ViewportWidget;
	
	TWeakPtr<class FSsPlayer> Player;


public:
	TSharedPtr<class FSsProjectViewerViewportClient> ViewportClient;

	void ToPrevFrame();
	void ToNextFrame();

};
