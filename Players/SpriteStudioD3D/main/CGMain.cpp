/*
 * @概要		ウインドウ管理クラス
 *
 * $Id: $
 *
 * @author		諒
 * @since		2014/07/18
 */

#include "stdafx.h"
#include <D3D9.h>
#include <D3DX9.h>
#include <Commdlg.h>
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

bool CGMain::OpenSSPJ( HWND hWnd )
{
	OPENFILENAME OpenFileName = { 0 };
	char szName[ MAX_PATH ];
	memset( szName, 0, MAX_PATH );
	OpenFileName.lStructSize = sizeof( OpenFileName );
	OpenFileName.hwndOwner = hWnd;
	OpenFileName.lpstrFilter = "SpriteStudioProject\0*.sspj\0";
	OpenFileName.nFilterIndex = 1;
	OpenFileName.lpstrFile = szName;
	OpenFileName.nMaxFile = MAX_PATH;
	OpenFileName.lpstrInitialDir = "";
	OpenFileName.lpstrTitle = "Select project file";
	OpenFileName.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	if( ::GetOpenFileName( &OpenFileName ) == 0) { return false; }
	if( m_currentrender != NULL ){ delete m_currentrender; }
	if( m_texfactory != NULL ){ delete m_texfactory; }
	if( m_proj != NULL ){ delete m_proj; }
	if( m_player != NULL ){ delete m_player; }
	m_currentrender = new SsCurrentRenderer( new SsRenderCCS( m_pDirect3D ) );
	m_texfactory = new SSTextureFactory( new SSTextureCCS( m_pDirect3D ) );
	m_proj = ssloader_sspj::Load( szName );
	if( m_proj == NULL ){ return false; }
	m_player = new SsAnimeDecoder();
	m_cellmap = new SsCellMapList();
	SsRenderCCS::changeAnimation( 0, 0, m_player, m_cellmap, m_proj );
	m_curanime = 0;
	m_frame = 0;
	m_add = m_player->getAnimeFPS() / 60.0f;
	m_nEndFrame = m_player->getAnimeEndFrame();
	// メニュー処理
	{
		HMENU hMenu;
		// SSAE
		hMenu = ::GetSubMenu( ::GetMenu( hWnd ), 1 );
		while( ::GetMenuItemCount( hMenu ) != 0 ){
			::DeleteMenu( hMenu, 0, MF_BYPOSITION );
		}
		// Amine
		hMenu = ::GetSubMenu( ::GetMenu( hWnd ), 2 );
		while( ::GetMenuItemCount( hMenu ) != 0 ){
			::DeleteMenu( hMenu, 0, MF_BYPOSITION );
		}
		// SSAE追加
		hMenu = ::GetSubMenu( ::GetMenu( hWnd ), 1 );
		SsAnimePackList AnimePackList = m_proj->getAnimePackList();
		if( AnimePackList.size() == 0 ){ return false; }
		for( UINT i = 0 ; i < AnimePackList.size() ; i++ ){
			::AppendMenu( hMenu, 0, WM_SELECTSSAE + i, AnimePackList[ i ]->name.c_str() );
		}
		// Anime追加
		hMenu = ::GetSubMenu( ::GetMenu( hWnd ), 2 );
		std::vector< SsAnimation* >& AnimeList = AnimePackList[ 0 ]->animeList;
		if( AnimeList.size() == 0 ){ return false; }
		for( UINT i = 0 ; i < AnimeList.size() ; i++ ){
			::AppendMenu( hMenu, 0, WM_SELECTANIM + i, AnimeList[ i ]->name.c_str() );
		}
	}

	return true;
}

// 初期化処理
bool CGMain::Init( void )
{
	// -------- DirectX初期化 --------
	CDirect3DHelper::InitDirect3DObject();
	m_pDirect3D.Attach( new CDirect3DHelper() );
	if( !m_pDirect3D->InitDirect3DDevice( this->GetWindow(), D3DDEVICE_TNL, m_WindowSize.x, m_WindowSize.y, true ) ){
		return 0;
	}
	// -------------------------------
	return true;
}

// アイドル時に行う処理
bool CGMain::Idle( void )
{
	// ================================================================
	// Calc
	// ================================================================
	{
		if( m_nEndFrame != 0 ){
			m_frame += m_add;
			m_frame = fmodf( m_frame, static_cast< float >( m_nEndFrame ) );
		}
		if( m_player != NULL ){
			m_player->setPlayFrame( m_frame );
			m_player->update();
		}
	}

	// ================================================================
	// Draw
	// ================================================================
	{
		m_pDirect3D->ClearScreen( 0xFF4488FF );
		m_pDirect3D->BeginDraw();
		if( m_player != NULL ){
			SsRenderCCS* render = static_cast< SsRenderCCS* >( SsCurrentRenderer::getRender() );
			float scaleX = 1.0f;
			float scaleY = 1.0f;
			float destX = 320.0f;
			float destY = 240.0f;
			float matrix[ 16 ] = {
				scaleX, 0.0f,   0.0f, 0.0f,
				0.0f,   scaleY, 0.0f, 0.0f,
				0.0f,   0.0f,   1.0f, 0.0f,
				destX,  destY,  0.0f, 1.0f,
			};
			// 最終スケール・最終オフセット
			render->setMatrix( &matrix[ 0 ] );
			m_player->draw();
			render->getDrawer()->Rendering();
		}
		m_pDirect3D->EndDraw();
		m_pDirect3D->Flip();
	}
	return true;
}

// 終了処理
void CGMain::Term( void )
{
	delete m_player;
	delete m_proj;
	delete m_texfactory;
	delete m_currentrender;
	// --------- DirectX後処理 --------
	m_pDirect3D->TermDirect3DDevice();
	CDirect3DHelper::TermDirect3DObject();
	// --------------------------------
}

LRESULT CGMain::WinProc( HWND hWnd, UINT msg, WPARAM WParam, LPARAM LParam )
{
	if( msg == WM_COMMAND ){
		switch( LOWORD( WParam ) ){
		case ID_FILE_OPENSSPJ:
			OpenSSPJ( hWnd );
			break;
		case ID_FILE_QUIT:
			::SendMessage( hWnd, WM_CLOSE, 0, 0 );
			break;
		default:
			break;
		}
		// メニュー処理
		for( WORD i = WM_SELECTSSAE ; i < WM_SELECTSSAE + 0x0FF ; i++ ){
			if( LOWORD( WParam ) == i ){
				m_current_pack = i - WM_SELECTSSAE;
				SsRenderCCS::changeAnimation( m_current_pack, 0, m_player, m_cellmap, m_proj );
				m_curanime = 0;
				m_frame = 0;
				m_add = m_player->getAnimeFPS() / 60.0f;
				m_nEndFrame = m_player->getAnimeEndFrame();
				// Amine
				HMENU hMenu = ::GetSubMenu( ::GetMenu( hWnd ), 2 );
				while( ::GetMenuItemCount( hMenu ) != 0 ){
					::DeleteMenu( hMenu, 0, MF_BYPOSITION );
				}
				// Anime追加
				hMenu = ::GetSubMenu( ::GetMenu( hWnd ), 2 );
				SsAnimePackList AnimePackList = m_proj->getAnimePackList();
				std::vector< SsAnimation* >& AnimeList = AnimePackList[ m_current_pack ]->animeList;
				if( AnimeList.size() == 0 ){ return false; }
				for( UINT i = 0 ; i < AnimeList.size() ; i++ ){
					::AppendMenu( hMenu, 0, WM_SELECTANIM + i, AnimeList[ i ]->name.c_str() );
				}
			}
		}
		for( WORD i = WM_SELECTANIM ; i < WM_SELECTANIM + 0x0FF ; i++ ){
			if( LOWORD( WParam ) == i ){
				int j = i - WM_SELECTANIM;
				SsRenderCCS::changeAnimation( m_current_pack, j, m_player, m_cellmap, m_proj );
				m_curanime = 0;
				m_frame = 0;
				m_add = m_player->getAnimeFPS() / 60.0f;
				m_nEndFrame = m_player->getAnimeEndFrame();
			}
		}

	}
	return CGWindowBase::WinProc( hWnd, msg, WParam, LParam );
}
