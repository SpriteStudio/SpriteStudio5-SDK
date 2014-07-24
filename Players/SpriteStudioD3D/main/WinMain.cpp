/*
 * @概要		WinMain
 *
 * $Id: c3d06c5a59e39c2bce12d0b152d69b040e762216 $
 *
 * @author		諒
 * @since		2014/07/10
 */

#include "Stdafx.h"

#include <D3D9.h>
#include <D3DX9.h>
#include "../Helper/ICCSInterface.h"
#include "../Helper/Direct3D.h"
#include "../Helper/CGlobalObject.h"
#include "../Helper/CGWindowBase.h"
#include "../Helper/CD3DTexture.h"
#include "../Helper/CD3DRender.h"

#include "../spritestudio/loader/tinyxml2/tinyxml2.h"
#include "../spritestudio/loader/ssloader.h"
#include "../spritestudio/helper/debugprint.h"
#include "../spritestudio/animator/ssplayer_animedecode.h"
#include "../spritestudio/render/ssplayer_render_ccs.h"
#include "../spritestudio/render/ssplayer_texture_ccs.h"

#include "CGMain.h"
#include "../Resource.h"

#pragma warning( disable:4100 )

void CreateWindow( CGMain* pWindow, HMENU hMenu, int nWidth, int nHeight );

//======================================================================
// メイン関数
//======================================================================
int APIENTRY WinMain( HINSTANCE hInst, HINSTANCE hPrev, LPSTR pszCmd, int nShow )
{
	_CrtSetDbgFlag( _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF );

	// -------- 共通リソース管理 --------
	CComPtr< CGlobalObject > pGlobal;
	pGlobal.Attach( new CGlobalObject() );
	CGlobalObject::GetThis()->SetInstance( hInst );
	// ----------------------------------

	// -------- ウィンドウ管理 --------
	CComPtr< CGMain > pWindow;
	pWindow.Attach( new CGMain() );
	HMENU hMenu = ::LoadMenu( hInst, reinterpret_cast< const char* >( IDR_MENU1 ) );
	CreateWindow( pWindow, hMenu, 640, 480 );
	// --------------------------------

	// 実行
	pWindow->Run();

	return 0;
}

//======================================================================
// ウィンドウ作成用サブルーチン
//======================================================================
void CreateWindow( CGMain* pWindow, HMENU hMenu, int nWidth, int nHeight )
{
	POINT WindowSize = { nWidth, nHeight };
	pWindow->SetWindowSize( WindowSize );
	int nWindow_W = WindowSize.x + ::GetSystemMetrics( SM_CXEDGE ) + ::GetSystemMetrics( SM_CXBORDER ) + ::GetSystemMetrics( SM_CXDLGFRAME );
	int nWindow_H = WindowSize.y + ::GetSystemMetrics( SM_CYEDGE ) + ::GetSystemMetrics( SM_CYBORDER ) + ::GetSystemMetrics( SM_CYDLGFRAME ) + ::GetSystemMetrics( SM_CYCAPTION );
	if( hMenu != NULL ){
		// メニュー分高さを追加
		nWindow_H += ::GetSystemMetrics( SM_CYMENU );
	}
	int nWindow_L = ::GetSystemMetrics( SM_CXSCREEN ) / 2 - nWindow_W / 2;
	int nWindow_T = ::GetSystemMetrics( SM_CYSCREEN ) / 2 - nWindow_H / 2;
	pWindow->CreateWindow(
		"GameWindow",
		"SpriteStudioTest",
		WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
		nWindow_L,
		nWindow_T,
		nWindow_W,
		nWindow_H,
		NULL,
		::LoadCursor( NULL, IDC_ARROW ),
		reinterpret_cast< HBRUSH >( ::GetStockObject( BLACK_BRUSH ) ),
		hMenu,
		SW_SHOW
	);
}

#pragma warning( default:4100 )
