/*
 * @概要		ウインドウ管理クラス
 *
 * $Id: $
 *
 * @author		諒
 * @since		2014/07/18
 */

#pragma once

class CGMain:public CGWindowBase{
public:
	CGMain( void ):
		CGWindowBase(),
		m_nRefCount( 1 )
	{
		m_currentrender = NULL;
		m_texfactory = NULL;
		m_proj = NULL;
		m_player = NULL;
		m_cellmap = NULL;

		m_current_pack = 0;
		m_curanime = 0;
		m_frame = 0;
		m_add = 0;
		m_nEndFrame = 0;
	}
	virtual ~CGMain( void ){}

public:
	virtual ULONG STDCALLMETHOD AddRef( void )
	{
		return ++m_nRefCount;
	}
	virtual ULONG STDCALLMETHOD Release( void )
	{
		if( --m_nRefCount == 0 ){
			delete this;
			return 0;
		}
		return m_nRefCount;
	}
	virtual ULONG STDCALLMETHOD GetRefCount( void )
	{
		return m_nRefCount;
	}

public:
	// 初期化処理
	virtual bool Init( void );
	// アイドル時に行う処理
	virtual bool Idle( void );
	// 終了処理
	virtual void Term( void );

	void SetWindowSize( const POINT& Point )
	{
		m_WindowSize = Point;
	}

	virtual LRESULT	WinProc( HWND hWnd, UINT msg, WPARAM WParam, LPARAM LParam );

private:
	bool OpenSSPJ( HWND hWnd );

private:
	int m_nRefCount;
	CComPtr< CDirect3DHelper > m_pDirect3D;
	POINT m_WindowSize;

	SsCurrentRenderer* m_currentrender;
	SSTextureFactory* m_texfactory;
	SsProject* m_proj;
	SsAnimeDecoder* m_player;
	SsCellMapList* m_cellmap;// SsAnimeDecoderが自動開放する

	int m_current_pack;
	int m_curanime;
	float m_frame;
	float m_add;
	int m_nEndFrame;

	static const DWORD WM_SELECTSSAE = 0x100;
	static const DWORD WM_SELECTANIM = 0x200;
};
