/*
 * @概要		ウインドウベースクラス
 *
 * $Id: CGWindowBase.h 1090 2008-01-08 03:56:49Z ryoh-srs $
 *
 * @author		諒
 * @since		2003/08/23
 */

#pragma once
#undef CreateWindow
#undef DestroyWindow
#undef GetWindow

// ======================================================================
//  クラス宣言
// ======================================================================
class DLL_EXPORT CGWindowBase:public CCS::ICCSInterface{
public:
	// コンストラクタ・デストラクタ
	CGWindowBase::CGWindowBase( void ):
		m_hWnd( NULL ),
		m_hMenu( NULL ),
		m_bIdle( true ){}
	virtual ~CGWindowBase( void ){}

public:
	// ウインドウハンドルを取得
	virtual HWND GetWindow( void )const
	{
		return m_hWnd;
	}

	virtual HMENU GetMenu( void )const
	{
		return m_hMenu;
	}

	virtual bool	CreateWindow( const std::string& sClassName, const std::string& sWindowTitle, DWORD dwStyle, int x, int y, int w, int h, HICON hIcon, HCURSOR hCursor, HBRUSH hBrush, HMENU hMenu, int nShow );
	virtual void	Run( void );
	virtual void	DestroyWindow( void );

protected:
	// 初期化処理
	virtual bool Init( void ) = 0;
	// アイドル時に行う処理
	virtual bool Idle( void ) = 0;
	// 終了処理
	virtual void Term( void ) = 0;
	// メッセージ処理
	virtual LRESULT	WinProc( HWND hWnd, UINT msg, WPARAM WParam, LPARAM LParam );

private:
	static LRESULT CALLBACK WinProc_Inner( HWND hWnd, UINT msg, WPARAM WParam, LPARAM LParam );

protected:
	HWND	m_hWnd;
	HMENU	m_hMenu;
	bool	m_bIdle;
};
