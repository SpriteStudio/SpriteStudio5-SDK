/*
 * @概要		DirectX管理クラス
 *
 * $Id: $
 *
 * @author		諒
 * @since		2014/07/18
 */

#include "stdafx.h"

#include <D3D9.h>
#include <D3DX9.h>
#include "Direct3D.h"

CComPtr< IDirect3D9 > CDirect3DHelper::g_pDirect3D;

void CDirect3DHelper::InitDirect3DObject( void )
{
	g_pDirect3D.Attach( ::Direct3DCreate9( D3D_SDK_VERSION ) );
}

void CDirect3DHelper::TermDirect3DObject( void )
{
	g_pDirect3D.Release();
}

bool CDirect3DHelper::InitDirect3DDevice( HWND hWnd, D3DDEVICEMODE DevMode, UINT nWidth, UINT nHeight, bool bVSync )
{
	// 初期化に必要な情報を得る
	D3DCAPS9 DeviceCaps;
	if( FAILED( g_pDirect3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DeviceCaps ) ) ){
		// デバイス情報が取得できない
		return false;
	}
	// 各種パラメータ設定
	m_ConstParameter.nMaxTextureWidth	= DeviceCaps.MaxTextureWidth;
	m_ConstParameter.nMaxTextureHeight	= DeviceCaps.MaxTextureHeight;
	m_ConstParameter.nMaxPrimitiveCount	= DeviceCaps.MaxPrimitiveCount;
	m_ConstParameter.nMaxVertexIndex	= DeviceCaps.MaxVertexIndex;
	m_ConstParameter.bTextureSizePow2	= ( DeviceCaps.TextureCaps & D3DPTEXTURECAPS_POW2 ) != 0;
	// Direct3D 初期化パラメータの設定
	D3DPRESENT_PARAMETERS Zero_present = { 0 };
	m_D3DParam = Zero_present;
	m_D3DParam.MultiSampleType				= D3DMULTISAMPLE_NONE;
	m_D3DParam.MultiSampleQuality			= 0;
	m_D3DParam.SwapEffect					= D3DSWAPEFFECT_DISCARD;
	m_D3DParam.BackBufferCount				= 1;
	m_D3DParam.hDeviceWindow				= hWnd;
	m_D3DParam.EnableAutoDepthStencil		= TRUE;
	m_D3DParam.Windowed						= TRUE;
	m_D3DParam.AutoDepthStencilFormat		= D3DFMT_D16;
	m_D3DParam.BackBufferWidth				= nWidth;
	m_D3DParam.BackBufferHeight				= nHeight;
	m_D3DParam.FullScreen_RefreshRateInHz	= 0;
	m_D3DParam.BackBufferFormat				= D3DFMT_X8R8G8B8;
	if( bVSync ){
		m_D3DParam.PresentationInterval	= D3DPRESENT_INTERVAL_ONE;
	}else{
		m_D3DParam.PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;
	}

	IDirect3DDevice9* p;
	// デバイスの生成
	switch( DevMode ){
	case D3DDEVICE_TNL:
		// Hardware TnL
		if( ( DeviceCaps.DevCaps & D3DDEVCAPS_HWRASTERIZATION )
				&& ( DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
				&& ( DeviceCaps.DevCaps & D3DDEVCAPS_PUREDEVICE )
				&& ( DeviceCaps.VertexShaderVersion >= D3DVS_VERSION( 1, 1 ) ) ){
			if( FAILED( g_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE | D3DCREATE_MULTITHREADED, &m_D3DParam, &p ) ) ){
				return false;
			}
		}else{
			return false;
		}
		break;
	case D3DDEVICE_HAL:
		// Hardware Accelerator Layer
		if( ( DeviceCaps.DevCaps & D3DDEVCAPS_HWRASTERIZATION )
				&& ( DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) ){
			if( FAILED( g_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &m_D3DParam, &p ) ) ){
				return false;
			}
		}else{
			return false;
		}
		break;
	case D3DDEVICE_HEL:
		// Hardware Emulation Layer
		if( DeviceCaps.DevCaps & D3DDEVCAPS_HWRASTERIZATION ){
			if( FAILED( g_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &m_D3DParam, &p ) ) ){
				return false;
			}
		}else{
			return false;
		}
		break;
	default:
		// リファレンスモードはおそらく使用できるレベルではないので省略してエラーを返す
		return false;
	}
	// デバイスの割付
	m_pD3DDevice.Attach( p );
	return true;
}

void CDirect3DHelper::TermDirect3DDevice( void )
{
	m_pD3DDevice.Release();	
}

// スクリーンを初期化する
void STDCALLMETHOD CDirect3DHelper::ClearScreen( DWORD dwBackColor )
{
	m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, dwBackColor, 1.0f, 0 );
}

// 画面を転送
bool STDCALLMETHOD CDirect3DHelper::Flip( void )
{
	if( m_pD3DDevice != NULL ){
		return m_pD3DDevice->Present( NULL, NULL, NULL, NULL ) != D3DERR_DEVICELOST;
	}
	return false;
}

// 描画処理開始
bool STDCALLMETHOD CDirect3DHelper::BeginDraw( void )
{
	// レンダリング方法の設定
	if( m_pD3DDevice != NULL ){
		// 頂点フォーマットの設定
		m_pD3DDevice->SetVertexShader( NULL );
		m_pD3DDevice->SetFVF( VERTEXFVF );
		// ポリゴンの裏を表示
		SetCullMode( D3DCULL_NONE );
		// ライトは使用せず
		SetLighting( FALSE );
		// 乗算
		SetTexColorOp( D3DTOP_MODULATE );
		SetTexColorArg1( D3DTA_TEXTURE );
		SetTexColorArg2( D3DTA_DIFFUSE );
		// αの算出方法の設定
		SetTexAlphaOp( D3DTOP_MODULATE );
		SetTexAlphaArg1( D3DTA_TEXTURE );
		SetTexAlphaArg2( D3DTA_DIFFUSE );
		// 通常ブレンド
		SetBlendOp( D3DBLENDOP_ADD );
		SetSrcBlend( D3DBLEND_SRCALPHA );
		SetDstBlend( D3DBLEND_INVSRCALPHA );
		// 合成後のα値の算出方法の設定
		SetBlendOpAlpha( D3DBLENDOP_ADD );
		SetSrcBlendAlpha( D3DBLEND_ONE );
		SetDstBlendAlpha( D3DBLEND_ZERO );
		// αブレンドは無効
		SetAlphaBlendEnable( FALSE );
		SetSeparateAlphaBlendEnable( FALSE );
		// αテストは無効
		SetAlphaTestEnable( FALSE );
		// バイリニアフィルターはオフ
		SetScaleState( D3DTEXF_POINT );
		// 描画準備開始
		return m_pD3DDevice->BeginScene() == D3D_OK;
	}
	return false;
}

// 描画処理終了
void STDCALLMETHOD CDirect3DHelper::EndDraw( void )
{
	if( m_pD3DDevice != NULL ){
		m_pD3DDevice->EndScene();
	}
}

void CDirect3DHelper::SetBlendOp( DWORD d )
{
	if( m_nBlendOp != d ){
		m_nBlendOp = d;
		m_pD3DDevice->SetRenderState( D3DRS_BLENDOP, d );
	}
}

void CDirect3DHelper::SetSrcBlend( DWORD d )
{
	if( m_nSrcBlend != d ){
		m_nSrcBlend = d;
		m_pD3DDevice->SetRenderState( D3DRS_SRCBLEND, d );
	}
}

void CDirect3DHelper::SetDstBlend( DWORD d )
{
	if( m_nDstBlend != d ){
		m_nDstBlend = d;
		m_pD3DDevice->SetRenderState( D3DRS_DESTBLEND, d );
	}
}

void CDirect3DHelper::SetBlendOpAlpha( DWORD d )
{
	if( m_nBlendOpAlpha != d ){
		m_nBlendOpAlpha = d;
		m_pD3DDevice->SetRenderState( D3DRS_BLENDOPALPHA, d );
	}
}

void CDirect3DHelper::SetSrcBlendAlpha( DWORD d )
{
	if( m_nSrcBlendAlpha != d ){
		m_nSrcBlendAlpha = d;
		m_pD3DDevice->SetRenderState( D3DRS_SRCBLENDALPHA, d );
	}
}

void CDirect3DHelper::SetDstBlendAlpha( DWORD d )
{
	if( m_nDstBlendAlpha != d ){
		m_nDstBlendAlpha = d;
		m_pD3DDevice->SetRenderState( D3DRS_DESTBLENDALPHA, d );
	}
}

void CDirect3DHelper::SetAlphaBlendEnable( DWORD d )
{
	if( m_nAlphaBlendEnable != d ){
		m_nAlphaBlendEnable = d;
		m_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, d );
	}
}

void CDirect3DHelper::SetSeparateAlphaBlendEnable( DWORD d )
{
	if( m_nSeparateAlphaBlendEnable != d ){
		m_nSeparateAlphaBlendEnable = d;
		m_pD3DDevice->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, d );
	}
}

void CDirect3DHelper::SetCullMode( DWORD d )
{
	if( m_nCullMode != d ){
		m_nCullMode = d;
		m_pD3DDevice->SetRenderState( D3DRS_CULLMODE, d );
	}
}

void CDirect3DHelper::SetLighting( DWORD d )
{
	if( m_nLighting != d ){
		m_nLighting = d;
		m_pD3DDevice->SetRenderState( D3DRS_LIGHTING, d );
	}
}

void CDirect3DHelper::SetAlphaTestEnable( DWORD d )
{
	if( m_nAlphaTestEnable != d ){
		m_nAlphaTestEnable = d;
		m_pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, d );
	}
}

void CDirect3DHelper::SetAlphaRef( DWORD d )
{
	if( m_nAlphaRef != d ){
		m_nAlphaRef = d;
		m_pD3DDevice->SetRenderState( D3DRS_ALPHAREF, d );
	}
}

void CDirect3DHelper::SetAlphaFunc( DWORD d )
{
	if( m_nAlphaFunc != d ){
		m_nAlphaFunc = d;
		m_pD3DDevice->SetRenderState( D3DRS_ALPHAFUNC, d );
	}
}

void CDirect3DHelper::SetTexColorOp( DWORD d )
{
	if( m_nTexColorOp != d ){
		m_nTexColorOp = d;
		m_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, d );
	}
}

void CDirect3DHelper::SetTexColorArg1( DWORD d )
{
	if( m_nTexColorArg1 != d ){
		m_nTexColorArg1 = d;
		m_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, d );
	}
}

void CDirect3DHelper::SetTexColorArg2( DWORD d )
{
	if( m_nTexColorArg2 != d ){
		m_nTexColorArg2 = d;
		m_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, d );
	}
}

void CDirect3DHelper::SetTexAlphaOp( DWORD d )
{
	if( m_nTexAlphaOp != d ){
		m_nTexAlphaOp = d;
		m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, d );
	}
}

void CDirect3DHelper::SetTexAlphaArg1( DWORD d )
{
	if( m_nTexAlphaArg1 != d ){
		m_nTexAlphaArg1 = d;
		m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, d );
	}
}

void CDirect3DHelper::SetTexAlphaArg2( DWORD d )
{
	if( m_nTexAlphaArg2 != d ){
		m_nTexAlphaArg2 = d;
		m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, d );
	}
}

void CDirect3DHelper::SetScaleState( DWORD d )
{
	if( m_nScaleState != d ){
		m_nScaleState = d;
		m_pD3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, d );
		m_pD3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, d );
	}
}

void CDirect3DHelper::InitDeviceState( void )
{
	//次回Set時にSetRenderState/SetTextureStageStateを呼ぶように、
	//ありえない値をセットしておく。
	m_nCullMode			= 0x7FFFFFFF;
	m_nLighting			= 0x7FFFFFFF;
	m_nTexColorOp		= 0x7FFFFFFF;
	m_nTexColorArg1		= 0x7FFFFFFF;
	m_nTexColorArg2		= 0x7FFFFFFF;
	m_nTexAlphaOp		= 0x7FFFFFFF;
	m_nTexAlphaArg1		= 0x7FFFFFFF;
	m_nTexAlphaArg2		= 0x7FFFFFFF;
	m_nBlendOp			= 0x7FFFFFFF;
	m_nSrcBlend			= 0x7FFFFFFF;
	m_nDstBlend			= 0x7FFFFFFF;
	m_nBlendOpAlpha		= 0x7FFFFFFF;
	m_nSrcBlendAlpha	= 0x7FFFFFFF;
	m_nDstBlendAlpha	= 0x7FFFFFFF;
	m_nAlphaBlendEnable	= 0x7FFFFFFF;
	m_nAlphaTestEnable	= 0x7FFFFFFF;
	m_nAlphaRef			= 0x7FFFFFFF;
	m_nAlphaFunc		= 0x7FFFFFFF;
	m_nScaleState		= 0x7FFFFFFF;
}

// 回転行列の作成
void CDirect3DHelper::MatrixRotation( D3DMATRIX* pDMatrix, float fAngleX, float fAngleY, float fAngleZ )
{
	// 角度設定
	float fSX = sin( fAngleX );
	float fSY = sin( fAngleY );
	float fSZ = sin( fAngleZ );
	float fCX = cos( fAngleX );
	float fCY = cos( fAngleY );
	float fCZ = cos( fAngleZ );

	// 行列作成( X )
	pDMatrix->m[ 0 ][ 0 ] = fCZ * fCY;
	pDMatrix->m[ 0 ][ 1 ] = fCZ * fSY * fSX - fSZ * fCX;
	pDMatrix->m[ 0 ][ 2 ] = fCZ * fSY * fCX + fSZ * fSX;
	pDMatrix->m[ 0 ][ 3 ] = 0;
	// 行列作成( Y )
	pDMatrix->m[ 1 ][ 0 ] = fSZ * fCY;
	pDMatrix->m[ 1 ][ 1 ] = fSZ * fSY * fSX + fCZ * fCX;
	pDMatrix->m[ 1 ][ 2 ] = fSZ * fSY * fCX - fCZ * fSX;
	pDMatrix->m[ 1 ][ 3 ] = 0;
	// 行列作成( Z )
	pDMatrix->m[ 2 ][ 0 ] = -fSY;
	pDMatrix->m[ 2 ][ 1 ] = fCY * fSX;
	pDMatrix->m[ 2 ][ 2 ] = fCY * fCX;
	pDMatrix->m[ 2 ][ 3 ] = 0;
	// 行列作成( W )
	pDMatrix->m[ 3 ][ 0 ] = 0;
	pDMatrix->m[ 3 ][ 1 ] = 0;
	pDMatrix->m[ 3 ][ 2 ] = 0;
	pDMatrix->m[ 3 ][ 3 ] = 1;
}

// ベクターに行列を適用する
void CDirect3DHelper::TransformNormal( D3DVECTOR* pDVector, const D3DVECTOR* pSVector, const D3DMATRIX* pSMatrix )
{
	pDVector->x = pSVector->x * pSMatrix->m[ 0 ][ 0 ] + pSVector->y * pSMatrix->m[ 1 ][ 0 ] + pSVector->z * pSMatrix->m[ 2 ][ 0 ];
	pDVector->y = pSVector->x * pSMatrix->m[ 0 ][ 1 ] + pSVector->y * pSMatrix->m[ 1 ][ 1 ] + pSVector->z * pSMatrix->m[ 2 ][ 1 ];
	pDVector->z = pSVector->x * pSMatrix->m[ 0 ][ 2 ] + pSVector->y * pSMatrix->m[ 1 ][ 2 ] + pSVector->z * pSMatrix->m[ 2 ][ 2 ];
}
