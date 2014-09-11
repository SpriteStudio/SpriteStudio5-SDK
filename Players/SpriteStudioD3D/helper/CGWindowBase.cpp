/*
 * @概要		ウインドウベースクラス
 *
 * $Id: CGWindowBase.cpp 1094 2008-01-09 07:19:24Z ryoh-srs $
 *
 * @author		諒
 * @since		2003/08/23
 */

#include "Stdafx.h"
#include "ICCSInterface.h"
#include "CGWindowBase.h"
#include "CGlobalObject.h"

using namespace std;

// ウインドウを生成する
bool CGWindowBase::CreateWindow( const string& sClassName, const string& sWindowTitle, DWORD dwStyle, int x, int y, int w, int h, HICON hIcon, HCURSOR hCursor, HBRUSH hBrush, HMENU hMenu, int nShow )
{
	// 二重実行防止
	if( m_hWnd != NULL ){
		return false;
	}

	// ウィンドウクラスの登録
	WNDCLASSEX WindowClass = { 0 };
	WindowClass.cbSize			= sizeof( WNDCLASSEX );
	WindowClass.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	WindowClass.lpfnWndProc		= WinProc_Inner;
	WindowClass.hInstance		= CGlobalObject::GetThis()->GetInstance();
	WindowClass.hIcon			= hIcon;
	WindowClass.hCursor			= hCursor;
	WindowClass.hbrBackground	= hBrush;
	WindowClass.lpszClassName	= sClassName.c_str();
	if( !::RegisterClassEx( &WindowClass ) ){
		return false;
	}

	// ウィンドウ生成
	m_hWnd = ::CreateWindowEx(
		WS_EX_APPWINDOW,
		sClassName.c_str(),
		sWindowTitle.c_str(),
		dwStyle,
		x, y, w, h,
		NULL,
		NULL,
		CGlobalObject::GetThis()->GetInstance(),
		NULL );
	if( m_hWnd == NULL ){
		return false;
	}

	// メニューの割り当て
	m_hMenu = hMenu;
	if( m_hMenu != NULL ){
		::SetMenu( m_hWnd, m_hMenu );
	}

	// ウインドウオブジェクトの登録
	::SetProp( m_hWnd, "WindowObject", this );

	// ウインドウを表示する
	::ShowWindow( m_hWnd, nShow );
	::UpdateWindow( m_hWnd );

	// ここでWM_CREATEを発行する
	::SendMessage( m_hWnd, WM_CREATE, 0, 0 );

	return true;
}

// ウインドウのメインループ
void CGWindowBase::Run( void )
{
	// 初期化処理
	if( !Init() ){ return; }
	// メインループ
	for( ; ; ){
		MSG	msg;
		if( ::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ){
			if( msg.message == WM_QUIT ){ break; }
			// メッセージを処理
			m_bIdle = true;
			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
		}else if( m_bIdle ){
			// アイドル時の処理
			if( !Idle() ){
				m_bIdle = false;
			}
		}else{
			// メッセージを待つ
			::WaitMessage();
		}
	}
	// 終了処理
	Term();
}

// ウインドウを破棄する
void CGWindowBase::DestroyWindow( void )
{
	if( m_hWnd != NULL ){
		// ウインドウ設定を初期値に戻す
		::ShowWindow( m_hWnd, SW_HIDE );
		::SetWindowLong( m_hWnd, GWL_STYLE, WS_POPUP );
		::SetWindowPos( m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
		// ウインドウオブジェクトを破棄
		::RemoveProp( m_hWnd, "WindowObject" );
		// メニューの破棄
		if( m_hMenu != NULL ){
			::SetMenu( m_hWnd, NULL );
			::DestroyMenu( m_hMenu );
			m_hMenu = NULL;
		}
		// ウインドウを破棄
		::DestroyWindow( m_hWnd );
		m_hWnd = NULL;
	}
}

LRESULT CGWindowBase::WinProc( HWND hWnd, UINT msg, WPARAM WParam, LPARAM LParam )
{
	if( msg == WM_CLOSE ){
		// 終了メッセージを送信
		::PostQuitMessage( 0 );
		return 0;
	}
	return ::DefWindowProc( hWnd, msg, WParam, LParam );
}

// コールバック処理
LRESULT CALLBACK CGWindowBase::WinProc_Inner( HWND hWnd, UINT msg, WPARAM WParam, LPARAM LParam )
{
	// ウインドウオブジェクトの取得
	CGWindowBase* pMWindow = static_cast< CGWindowBase* >( ::GetProp( hWnd, "WindowObject" ) );
	// 取得できればオブジェクト実行、出来なければデフォルトの処理
	if( pMWindow != NULL ){
		return pMWindow->WinProc( hWnd, msg, WParam, LParam );
	}
	return ::DefWindowProc( hWnd, msg, WParam, LParam );
}
