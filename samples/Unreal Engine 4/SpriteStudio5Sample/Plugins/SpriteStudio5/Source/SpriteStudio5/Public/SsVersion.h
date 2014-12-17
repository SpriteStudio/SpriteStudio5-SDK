#pragma once

//
// 対応UEバージョン切り替え用
//
#define SS_UE4_4
//#define SS_UE4_5
//#define SS_UE4_6


//
//	UE4.5/UE4.6 で使用する場合、エンジンコードを修正して↓の問題を解決する必要があります。 
//	この問題を解決しないと、SsPlayerコンポーネント作成後、即座にプログラムがクラッシュします。 
//		https://answers.unrealengine.com/questions/121155/45-urgent-solus-making-an-fcanvas-for-render-textu.html
//	最短の修正方法としては、CanvasRenderTarget2D.cpp UCanvasRenderTarget2D::UpdateResource() 内の下記の箇所を修正して下さい。 
//		// Create the FCanvas which does the actual rendering.
//		×：FCanvas RenderCanvas(GameThread_GetRenderTargetResource(), nullptr, FApp::GetCurrentTime() - GStartTime, FApp::GetDeltaTime(), FApp::GetCurrentTime() - GStartTime, GetWorld()->FeatureLevel);
//		○：FCanvas RenderCanvas(GameThread_GetRenderTargetResource(), nullptr, FApp::GetCurrentTime() - GStartTime, FApp::GetDeltaTime(), FApp::GetCurrentTime() - GStartTime, GMaxRHIFeatureLevel);
//
//	3Dメッシュへの描画を行わず、HUD用途のみであればそのままでも使用可能です。 
//	上記の問題を解決し、UE4.5/4.6で3Dメッシュへの描画を使用する場合は、SS_DISABLE_DRAW_MESHを無効化して下さい。 
//
#if defined(SS_UE4_5) || defined(SS_UE4_6)
#define SS_DISABLE_DRAW_MESH
#endif


//
//	UE4.5以降でビルドする場合は、上の#defineだけでなく、 SpriteStudio5Ed.Build.cs 44行目のコメントアウトを外して下さい 
//


//
//	描画が上下反転する不具合について 
//		UE4.4のビルドをAndroidの実機上で動かした際に、描画が上下反転する不具合が確認されています。 
//		この問題はUE4.5以降では修正されています。 
//			（参照：UE4.5以降のエンジンコードで「bNeedsToSwitchVerticalAxis」を検索） 
//		下記の#defineを有効にすると、このプラグインからの描画を上下反転します。 
//		但し、この修正は即席のもので、Windowsでの描画も反転していまいますので、ご了承下さい。 
//
//#define SS_SWITCH_VERTICAL_AXIS

