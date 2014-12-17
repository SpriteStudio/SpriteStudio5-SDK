#include "SpriteStudio5EdPrivatePCH.h"
#include "SsProjectViewerViewport.h"

#include "SsPlayer.h"
#include "SceneViewport.h"
#include "SsProjectViewerViewportClient.h"


void SSsProjectViewerViewport::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	if(Player.IsValid())
	{
		Player.Pin()->Tick(InDeltaTime);
	}

	Viewport->InvalidateDisplay();
}

void SSsProjectViewerViewport::Construct(const FArguments& InArgs)
{
	this->ChildSlot
	[
		SAssignNew(ViewportWidget, SViewport)
			.EnableGammaCorrection(false)
			.ShowEffectWhenDisabled(false)
			.EnableBlending(true)
	];

	ViewportClient = MakeShareable(new FSsProjectViewerViewportClient());

	Viewport = MakeShareable(new FSceneViewport(ViewportClient.Get(), ViewportWidget));

	ViewportWidget->SetViewportInterface( Viewport.ToSharedRef() );
}

void SSsProjectViewerViewport::SetPlayer(TWeakPtr<class FSsPlayer> InPlayer)
{
	Player = InPlayer;
	ViewportClient->SetPlayer(Player);
}

void SSsProjectViewerViewport::ToPrevFrame()
{
	if( Player.IsValid() )
	{
		Player.Pin()->ToPrevFrame();
	}
}
void SSsProjectViewerViewport::ToNextFrame()
{
	if( Player.IsValid() )
	{
		Player.Pin()->ToNextFrame();
	}
}