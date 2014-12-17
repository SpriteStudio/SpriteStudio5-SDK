#pragma once

class FSsProjectViewerViewportClient
	: public FViewportClient
	, public FGCObject
{

public:
	FSsProjectViewerViewportClient::FSsProjectViewerViewportClient();

	// FViewportClient Functions
	virtual void Draw(FViewport* Viewport, FCanvas* Canvas) override;

	// FGCObject Functions
	virtual void AddReferencedObjects( FReferenceCollector& Collector ) {}

public:
	void SetPlayer(TWeakPtr<class FSsPlayer> InPlayer);

private:
	void DrawGrid(FViewport* Viewport, FCanvas* Canvas, const FVector2D& Center);

public:
	bool bDrawGrid;
	int32 GridSize;
	FLinearColor GridColor;
	FLinearColor BackgroundColor;

private:
	TWeakPtr<class FSsPlayer> Player;
};
