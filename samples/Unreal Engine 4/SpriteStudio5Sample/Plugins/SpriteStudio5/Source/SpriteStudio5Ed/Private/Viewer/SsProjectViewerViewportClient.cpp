#include "SpriteStudio5EdPrivatePCH.h"
#include "SsProjectViewerViewportClient.h"

#include "Engine/Canvas.h"

#include "SsPlayer.h"


FSsProjectViewerViewportClient::FSsProjectViewerViewportClient()
	: bDrawGrid(true)
	, GridSize(50)
	, GridColor(FLinearColor::Green)
	, BackgroundColor(.2f, .2f, .2f)
{
}

void FSsProjectViewerViewportClient::Draw(FViewport* Viewport, FCanvas* Canvas)
{
	FIntPoint Size = Viewport->GetSizeXY();
	FVector2D Center(
		(float)(Size.X/2) + (Player.Pin()->GetAnimPivot().X * Size.X),
		(float)(Size.Y/2) - (Player.Pin()->GetAnimPivot().Y * Size.Y)
		);

	Canvas->Clear(BackgroundColor);
	if(bDrawGrid && (0 < GridSize))
	{
		DrawGrid(Viewport, Canvas, Center);
	}

	// Ss‚Ì•`‰æ
	Player.Pin()->Draw(Canvas, Center);
}

void FSsProjectViewerViewportClient::DrawGrid(FViewport* Viewport, FCanvas* Canvas, const FVector2D& Center)
{
	FIntPoint Size = Viewport->GetSizeXY();

	for(int32 X = Center.X; X <= Size.X; X+=GridSize)
	{
		FCanvasLineItem Line(FVector2D(X,0.f), FVector2D(X,Size.Y));
		Line.SetColor(GridColor);
		Canvas->DrawItem(Line);
	}
	for(int32 X = Center.X-GridSize; 0 <= X; X-=GridSize)
	{
		FCanvasLineItem Line(FVector2D(X,0.f), FVector2D(X,Size.Y));
		Line.SetColor(GridColor);
		Canvas->DrawItem(Line);
	}
	{
		FCanvasLineItem Line(FVector2D(Center.X+1,0.f), FVector2D(Center.X+1,Size.Y));
		Line.SetColor(GridColor);
		Canvas->DrawItem(Line);
	}
	{
		FCanvasLineItem Line(FVector2D(Center.X-1,0.f), FVector2D(Center.X-1,Size.Y));
		Line.SetColor(GridColor);
		Canvas->DrawItem(Line);
	}

	for(int32 Y = Center.Y; Y <= Size.Y; Y+=GridSize)
	{
		FCanvasLineItem Line(FVector2D(0.f,Y), FVector2D(Size.X,Y));
		Line.SetColor(GridColor);
		Canvas->DrawItem(Line);
	}
	for(int32 Y = Center.Y-GridSize; 0 <= Y; Y-=GridSize)
	{
		FCanvasLineItem Line(FVector2D(0.f,Y), FVector2D(Size.X,Y));
		Line.SetColor(GridColor);
		Canvas->DrawItem(Line);
	}
	{
		FCanvasLineItem Line(FVector2D(0.f,Center.Y+1), FVector2D(Size.X,Center.Y+1));
		Line.SetColor(GridColor);
		Canvas->DrawItem(Line);
	}
	{
		FCanvasLineItem Line(FVector2D(0.f,Center.Y-1), FVector2D(Size.X,Center.Y-1));
		Line.SetColor(GridColor);
		Canvas->DrawItem(Line);
	}
}

void FSsProjectViewerViewportClient::SetPlayer(TWeakPtr<class FSsPlayer> InPlayer)
{
	Player = InPlayer;
}
