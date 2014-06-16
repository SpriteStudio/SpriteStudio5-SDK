//#include "stdafx.h"
#include <d3d9.h>
#include "d3dsimple.h"


//スタティック変数
CDirectDrawSimple*	CDirectDrawSimple::myInst = 0;
bool			CDirectDrawSimple::m_DeviceLost = false;

static	bool	IntervalImmediate = false;

/* ******************************************************** */
//! DirectDrawを初期化
/*!

@retval	成否　（　TRUE , FALSE )

*/
BOOL CDirectDrawSimple::Cleanup()
{
	SAFE_RELEASE(m_pd3dDev);
	SAFE_RELEASE(m_pD3D);

	return TRUE;
}
/* ******************************************************** */
//! DirectDraw作成時のパラメータを作成
/*!

*/
void	CDirectDrawSimple::CreateCaps()
{

	// 表示系のデバイス能力を取得
	D3DCAPS9	HardwareCaps;
	HRESULT	hr = GetD3D()->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &HardwareCaps);
	if(0 != (HardwareCaps.PresentationIntervals & D3DPRESENT_INTERVAL_ONE))
	{	/* DirectXが垂直同期を自動で見ることが可能 */
		IntervalImmediate = false;	/* 垂直同期をDirectXに任せる */
	} else {	/* DirectXが垂直同期を自動で見ることが不可能 */
		IntervalImmediate = true;	/* 垂直同期を自前で行う */
	}

	::ZeroMemory(&m_d3dParam_window, sizeof(m_d3dParam_window));
	m_d3dParam_window.BackBufferWidth = m_dwWidth;
	m_d3dParam_window.BackBufferHeight = m_dwHeight;
	m_d3dParam_window.BackBufferFormat=D3DFMT_A8R8G8B8;
	m_d3dParam_window.BackBufferCount = 1;
	m_d3dParam_window.Windowed = TRUE;
	m_d3dParam_window.EnableAutoDepthStencil = TRUE;
	m_d3dParam_window.AutoDepthStencilFormat = D3DFMT_D24S8;
	m_d3dParam_window.SwapEffect = D3DSWAPEFFECT_DISCARD;
	if(true == IntervalImmediate)	m_d3dParam_window.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	else							m_d3dParam_window.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	::ZeroMemory(&m_d3dParam_full, sizeof(m_d3dParam_full));
	m_d3dParam_full.BackBufferWidth = m_dwWidth;
	m_d3dParam_full.BackBufferHeight = m_dwHeight;
	m_d3dParam_full.BackBufferFormat= D3DFMT_A8R8G8B8;
	m_d3dParam_full.BackBufferCount = 1;
	m_d3dParam_full.Windowed = FALSE;
	m_d3dParam_full.EnableAutoDepthStencil = TRUE;
	m_d3dParam_full.AutoDepthStencilFormat = D3DFMT_D24S8;
	m_d3dParam_full.SwapEffect = D3DSWAPEFFECT_DISCARD;
	if(true == IntervalImmediate)	m_d3dParam_window.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	else							m_d3dParam_window.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
}

void	CDirectDrawSimple::DeviceReset()
{

	m_d3dParam_window.BackBufferWidth = m_dwWidth;
	m_d3dParam_window.BackBufferHeight = m_dwHeight;
	m_d3dParam_full.BackBufferWidth = m_dwWidth;
	m_d3dParam_full.BackBufferHeight = m_dwHeight;
	m_d3dParam_current.BackBufferWidth = m_dwWidth;
	m_d3dParam_current.BackBufferHeight = m_dwHeight;

	//取得済みバッファのリリース
	SAFE_RELEASE( m_pBackBuffer );

	HRESULT	hr = m_pd3dDev->TestCooperativeLevel();

	if ( hr == S_OK || hr == D3DERR_DEVICENOTRESET )
	{
//		if ( hr == S_OK ) 
//			CommonLib::DEBUG_PRINTF( " TestCooperativeLevel : S_OK\n" );
//		if ( hr == D3DERR_DEVICENOTRESET ) 
//			CommonLib::DEBUG_PRINTF( " TestCooperativeLevel : D3DERR_DEVICENOTRESET\n" );


		//HRESULT hr;
		hr = m_pd3dDev->Reset( &m_d3dParam_current );

		if (FAILED(hr))
		{
			m_DeviceLost = true;
			if (hr == D3DERR_DEVICELOST)
			{
				//m_DeviceLost = true;
				return ;
			}else if( hr == D3DERR_DEVICENOTRESET ){
				//THROW_ERROR_MESSAGE( "Error : D3DERR_DEVICENOTRESET" );
				return ;					
			}else if ( hr == D3DERR_INVALIDCALL )
			{
				//THROW_ERROR_MESSAGE( "Error : D3DERR_INVALIDCALL" );
				return ;
			}else{
				//THROW_ERROR_MESSAGE( "Error : DirectX Reset" );
				return ;
			}
		}

		m_pd3dDev->GetBackBuffer(0, 0, 
			D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer);

		m_currentRenderTarget = m_pBackBuffer;
		(m_RestoreDeviceCallback)();

	}
}

void	CDirectDrawSimple::ChangeDisplayMode( DISPLAYMODE mode )
{
	if ( mode != m_current_displaymode )
	{
		//D3DPRESENT_PARAMETERS d3dParam;
		if ( mode == DISPLAYMODE_WINDOW )
		{
			m_d3dParam_current = m_d3dParam_window;
//			CommonLib::DEBUG_PRINTF("change DISPLAYMODE_WINDOW\n" );
		}else if (mode == DISPLAYMODE_FULL ){
			m_d3dParam_current = m_d3dParam_full;
			GetWindowRect(m_hwnd, &g_rectWindow);
//			CommonLib::DEBUG_PRINTF("change DISPLAYMODE_FULL\n" );
		}

		DeviceReset();

		m_current_displaymode = mode;
		//ウインドウの位置、大きさを調整
		if (mode == DISPLAYMODE_WINDOW)
		{
			SetWindowLong(m_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
			SetWindowPos(m_hwnd, HWND_NOTOPMOST,
			g_rectWindow.left, g_rectWindow.top,
			g_rectWindow.right - g_rectWindow.left,
			g_rectWindow.bottom - g_rectWindow.top,
			SWP_SHOWWINDOW);
		}

	}
}


/* ******************************************************** */
//! DirectDrawを初期化
/*!
@param	HWND
	ウインドウハンドル

@param	ScreenWidth
	スクリーン水平方向サイズ

@param	ScreenHeight
	スクリーン垂直方向サイズ

@retval	成否　（　TRUE , FALSE )

*/

BOOL CDirectDrawSimple::Create( void* hWnd , int ScreenWidth , int ScreenHeight )
{
	m_dwWidth = ScreenWidth;
	m_dwHeight = ScreenHeight;

	HWND WindowHandle = (HWND)hWnd ;
	/* <<< DirectX9インタフェースの作成 >>> */
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if ( m_pD3D == NULL ) return FALSE;
	D3DDISPLAYMODE	d3ddm;

	if(FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm))){
		return FALSE;
	}

	D3DCAPS9	caps;
	// デバイスの能力を取得する
	m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);


	m_VertexShaderVersion = caps.VertexShaderVersion;
	m_PixelShaderVersion = caps.PixelShaderVersion;

//		m_VertexShaderVersion = 0;
//		m_PixelShaderVersion = 0;

	//シェーダーバージョン
	if( m_VertexShaderVersion < D3DVS_VERSION( 2, 0) ) {}
	if( m_PixelShaderVersion < D3DPS_VERSION( 2, 0) ) {}


	CreateCaps();
	//D3DPRESENT_PARAMETERS d3dParam;

	if ( m_current_displaymode == DISPLAYMODE_WINDOW )
	{
		m_d3dParam_current = m_d3dParam_window;
	}else{
		m_d3dParam_current = m_d3dParam_full;
	}

	D3DDEVTYPE	devType = D3DDEVTYPE_HAL;

#if 1	/* MEMO: ドライバにリソース管理を任せる */
	m_Vertextype = D3DCREATE_MIXED_VERTEXPROCESSING;
	if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, devType, WindowHandle,
//									  D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &d3dParam, &m_pd3dDev)))
									D3DCREATE_MIXED_VERTEXPROCESSING, &m_d3dParam_current, &m_pd3dDev)))
	{

		m_Vertextype = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, devType, WindowHandle,
										D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_d3dParam_current, &m_pd3dDev)))
		{
			// 駄目ならソフトウェアで処理する
			m_Vertextype = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
			if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, devType, WindowHandle,
											D3DCREATE_SOFTWARE_VERTEXPROCESSING,
											&m_d3dParam_current, &m_pd3dDev)))
			{
				return FALSE;
			}else{
			}
		}
	}
#else	/* MEMO: DirectXのマネージャにリソース管理を任せる */
	m_Vertextype = D3DCREATE_MIXED_VERTEXPROCESSING;
	if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, devType, WindowHandle,
//									  D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &d3dParam, &m_pd3dDev)))
									D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT, &m_d3dParam_current, &m_pd3dDev)))
	{

		m_Vertextype = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, devType, WindowHandle,
										D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT, &m_d3dParam_current, &m_pd3dDev)))
		{
			// 駄目ならソフトウェアで処理する
			m_Vertextype = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
			if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, devType, WindowHandle,
											D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT,
											&m_d3dParam_current, &m_pd3dDev)))
			{
				return FALSE;
			}else{
			}
		}
	}
#endif

	m_pd3dDev->Clear(0L, NULL, D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
							0x00000000, 1.0f, 0L);

	m_pd3dDev->SetVertexShader(0);
	m_pd3dDev->SetPixelShader(0);

	m_pd3dDev->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);
	m_pd3dDev->SetRenderState(D3DRS_LIGHTING,FALSE);
	m_pd3dDev->SetTextureStageState(0,D3DTSS_COLOROP,  D3DTOP_MODULATE);
	m_pd3dDev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	m_pd3dDev->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);

	m_pd3dDev->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	m_pd3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_pd3dDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pd3dDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);  //SRCの設定
	m_pd3dDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);//DESTの設定
		
	m_pd3dDev->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_NONE);	//mipmap切り替え時のfilter
	m_pd3dDev->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_POINT);	//拡大時のfilter
	m_pd3dDev->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_POINT);	//縮小時のfilter
		
//		m_pd3dDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
//		m_pd3dDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	m_pd3dDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pd3dDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	m_pd3dDev->GetBackBuffer(0, 0, 
			D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer);

	m_currentRenderTarget = m_pBackBuffer;


    D3DVIEWPORT9 vp = {0,0, m_dwWidth,m_dwHeight, 0,1};
    m_pd3dDev->SetViewport(&vp);

    // Set D3D matrices
    D3DMATRIX matId = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
    m_pd3dDev->SetTransform(D3DTS_WORLD, &matId);
    m_pd3dDev->SetTransform(D3DTS_VIEW, &matId);
    D3DMATRIX matProj = { (float)m_dwHeight/m_dwWidth,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
    m_pd3dDev->SetTransform(D3DTS_PROJECTION, &matProj);

    // Disable lighting and culling
    m_pd3dDev->SetRenderState( D3DRS_LIGHTING, FALSE );
    m_pd3dDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    m_pd3dDev->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );

	return TRUE;
}
	
LPDIRECT3DSURFACE9 CDirectDrawSimple::getBackBuffer()
{
	if ( m_pBackBuffer  == 0)
	{
		m_pd3dDev->GetBackBuffer(0, 0, 
				D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer);
	}
	return m_pBackBuffer;
}

BOOL	CDirectDrawSimple::DeviceChk()
{

	if (m_DeviceLost)
	{
		HRESULT hr;
		Sleep(100); // 0.1秒待つ

		DeviceReset();

		hr  = m_pd3dDev->TestCooperativeLevel();
		if (FAILED(hr))
		{
			if (hr == D3DERR_DEVICELOST)
			{
				return TRUE; // デバイスはまだ失われている
			}
			return FALSE;	// デバイスの復元に失敗
		}

		m_DeviceLost = false;
		return TRUE;
	}
	return TRUE;	// 一応TRUEを返しておく
}

HRESULT	CDirectDrawSimple::BeginScene()
{
 	HRESULT	hr = S_OK;
	hr  = m_pd3dDev->TestCooperativeLevel();

	if ( hr == S_OK )
	{
		if ( m_pause ) return S_FALSE;

		hr = m_pd3dDev->Clear( 0, NULL, D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
					D3DCOLOR_XRGB(bgcolorR,bgcolorG,bgcolorB), 1.0f, 0 );

		return m_pd3dDev->BeginScene();
	}else{
		//だめだったとき
		if ( hr == D3DERR_DEVICELOST || hr == D3DERR_DEVICENOTRESET)
		{
			m_DeviceLost = true;
			DeviceChk();
			return S_FALSE;
		}else{
			//予期せぬエラー
			//CommonLib::ThrowErrorMessage( 0 , "DirectX : 予期せぬエラー\n" );
		}
	}

	return S_FALSE;
}


HRESULT	CDirectDrawSimple::EndScene()
{
 	HRESULT	hr = S_OK;
	if ( m_pause ) return S_FALSE;


	hr = m_pd3dDev->EndScene();

	//バックバッファからプライマリバッファへ転送
	if(true == IntervalImmediate)
	{	/* 自前で垂直同期を見る場合 */
		D3DRASTER_STATUS	RasterStatus;
		for( ; ; )
		{
			if( D3D_OK == m_pd3dDev->GetRasterStatus(0, &RasterStatus) )
			{	/* 取得成功 */
				if(TRUE == RasterStatus.InVBlank)	break;	/* ループから脱出 */
				/* MEMO: ここで他のスレッドに処理をあけようとしてSleepしないこと。 */
				/*       ウィンドウを作成しているスレッドなので、このスレッドが寝ると、WndProcまで寝てしまいSleepしている間メッセージが処理されずOS全体の処理速度まで落ちる */
			}
		}
	}
 	if(FAILED(hr = m_pd3dDev->Present( NULL, NULL, NULL, NULL )))
 	{
		//DeviceChk();
 	}
		
	return hr;
}

/*
void	Setup2DMatrix( int width , int height , float center_posx , float center_posy )
{

	CDirectDraw* inst = CDirectDraw::GetInst();

	D3DXMATRIX      matWorld;
	D3DXMATRIX      matView;
	D3DXMATRIX      matProj;

	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixIdentity( &matView );
	D3DXMatrixIdentity( &matProj );

	float   x   = ( 2.0f / width );
	float   y   = ( -2.0f / height);
	float   z   =  1.0f;
	float   u   = -1.0f;
	float   v   =  1.0f;

	matProj._11 = x;
	matProj._22 = y;
	matProj._33 = z;
	matProj._41 = u;
	matProj._42 = v;

	D3DXMatrixTranslation( &matView ,(width/2) - center_posx , 
							(height/2) - center_posy , 0.0f );

	GetD3DDevice()->SetTransform(D3DTS_WORLD,&matWorld);
	GetD3DDevice()->SetTransform(D3DTS_VIEW,&matView);
	GetD3DDevice()->SetTransform(D3DTS_PROJECTION,&matProj);
}

void	Setup2DMatrix( int width , int height )
{
	CDirectDraw* inst = CDirectDraw::GetInst();

	D3DXMATRIX      matWorld;
	D3DXMATRIX      matView;
	D3DXMATRIX      matProj;

	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixIdentity( &matView );
	D3DXMatrixIdentity( &matProj );

	float   x   =  2.0f / width;
	float   y   = -2.0f / height;

	float   z   =  1.0f;
	float   u   = -1.0f;
	float   v   =  1.0f;

	matProj._11 = x;
	matProj._22 = y;
	matProj._33 = z;
	matProj._41 = u;
	matProj._42 = v;

	GetD3DDevice()->SetTransform(D3DTS_WORLD,&matWorld);
	GetD3DDevice()->SetTransform(D3DTS_VIEW,&matView);
	GetD3DDevice()->SetTransform(D3DTS_PROJECTION,&matProj);
}

*/