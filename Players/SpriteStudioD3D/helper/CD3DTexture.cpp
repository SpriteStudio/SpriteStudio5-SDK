/*
 * @概要		テクスチャクラス
 *
 * $Id: 22c2f276b6880e5e9cc437d6457439b6bcf3bab3 $
 *
 * @author		諒
 * @since		2003/10/25
 */

#include "Stdafx.h"
#include <D3D9.h>
#include <D3DX9.h>
#include <DDraw.h>
#include "CD3DTexture.h"
#include "Direct3D.h"

using namespace std;

bool STDCALLMETHOD CD3DTexture::CreateTextureFromFile( const char* pszFileName )
{
	// 強制アンロック^^;
	UnlockTexture();
	if( !m_bCreateTex && !m_bLocked ){
		IDirect3DDevice9* pD3DDevice;
		pD3DDevice = static_cast< IDirect3DDevice9* >( m_pD3DHelper->GetDevice() );
		if( pD3DDevice == NULL ){ return false; }
		IDirect3DTexture9* p;
		if( FAILED( ::D3DXCreateTextureFromFile( pD3DDevice, pszFileName, &p ) ) ){
			return false;
		}
		m_pDestTexture.Attach( p );
		m_pSrcTexture = NULL;
		// 初期化完了
		m_bSetParam = true;
		m_bCreateTex = true;
		D3DSURFACE_DESC Desc;
		m_pDestTexture->GetLevelDesc( 0, &Desc );
		m_nWidth		= Desc.Width;
		m_nHeight		= Desc.Height;
		m_nTWidth		= Desc.Width;
		m_nTHeight		= Desc.Height;
		m_bRenderTarget	= false;
		m_bCompress		= Desc.Format == D3DFMT_DXT5;
	}
	return true;
}

// テクスチャを作成
bool STDCALLMETHOD CD3DTexture::CreateTexture( UINT nWidth, UINT nHeight, bool bRenderTarget, bool bCompress )
{
	// 強制アンロック^^;
	UnlockTexture();
	// 状態を確認して、怪しかったらなにもしない
	if( nWidth != 0 && nHeight != 0 ){		
		IDirect3DDevice9* pD3DDevice;
		pD3DDevice = static_cast< IDirect3DDevice9* >( m_pD3DHelper->GetDevice() );
		if( pD3DDevice == NULL ){ return false; }
		// 各種パラメータ
		m_nWidth		= nWidth;
		m_nHeight		= nHeight;
		m_bRenderTarget	= bRenderTarget;
		m_bCompress		= bCompress;
		// テクスチャのフォーマット設定
		D3DFORMAT ColorDepth;
		DWORD dwUsage;
		if( bRenderTarget ){
			ColorDepth = D3DFMT_A8R8G8B8;
			dwUsage = D3DUSAGE_RENDERTARGET;
			m_bCompress = false;
		}else if( !bCompress ){
			ColorDepth = D3DFMT_A8R8G8B8;
			dwUsage = 0;
		}else{
			ColorDepth = D3DFMT_DXT5;
			dwUsage = 0;
		}
		const D3DCONSTPARAMETER& ConstParameter = *m_pD3DHelper->GetConstParameter();
		if( ConstParameter.bTextureSizePow2 ){
			// テクスチャサイズを2の二乗倍でしか作成できないグラフィックカードでは、
			// 二乗倍で一番近いサイズで作成し、それを採用するようにする
			// ※DXT5が4の倍数のテクスチャになるので開始値を4にしてある
			for( m_nTWidth = 4 ; m_nTWidth < nWidth ; m_nTWidth <<= 1 ){}
			for( m_nTHeight = 4 ; m_nTHeight < nHeight ; m_nTHeight <<= 1 ){}
		}else{
			// それ以外のテクスチャはDXT5の為に4の倍数で作成を行うようにする
			for( m_nTWidth = 4 ; m_nTWidth < nWidth ; m_nTWidth += 4 ){}
			for( m_nTHeight = 4 ; m_nTHeight < nHeight ; m_nTHeight += 4 ){}
		}
		// 調整後のサイズが作成できる最大テクスチャサイズを超えていた場合
		// 作成できないのでエラー終了させる
		if( ConstParameter.nMaxTextureWidth < m_nTWidth || ConstParameter.nMaxTextureHeight < m_nTHeight ){
			return false;
		}
		// テクスチャの生成
		{
			IDirect3DTexture9* p;
			if( FAILED( pD3DDevice->CreateTexture( m_nTWidth, m_nTHeight, 1, dwUsage, ColorDepth, D3DPOOL_DEFAULT, &p, NULL ) ) ){
				return false;
			}
			m_pDestTexture.Attach( p );
		}
		// レンダリングターゲットの場合
		// ステンシルサーフェスも作成する
		if( m_bRenderTarget ){
			IDirect3DSurface9* p;
			if( FAILED( pD3DDevice->CreateDepthStencilSurface(
				m_nTWidth,
				m_nTHeight,
				D3DFMT_D16,
				D3DMULTISAMPLE_NONE,
				0,
				true,
				&p,
				NULL
				) ) ){
				return false;
			}
			m_pDepthSurface.Attach( p );
		}else{
			m_pDepthSurface = NULL;
		}
		// 古いシステムテクスチャはテクスチャの情報が変わった際に
		// 再利用できないので作成しなおす
		m_pSrcTexture = NULL;
		// 初期化完了
		m_bSetParam = true;
		m_bCreateTex = true;
		// ゴミが表示されるのを防止するため
		// テクスチャを黒く塗りつぶす^^;
		{
			D3DLOCKED_RECT LockRect;
			if( LockTexture( &LockRect, false ) ){
				if( !m_bCompress ){
					::ZeroMemory( LockRect.pBits, LockRect.Pitch * m_nTHeight );
				}else{
					::ZeroMemory( LockRect.pBits, LockRect.Pitch * ( m_nTHeight / 4 ) );
				}
				UnlockTexture();
			}
		}
	}
	return true;
}

// テクスチャを破棄する
void STDCALLMETHOD CD3DTexture::DestroyTexture( void )
{
	// 強制アンロック^^;
	UnlockTexture();
	if( m_bSetParam ){
		m_bSetParam			= false;
		m_bCreateTex		= false;
		m_pDepthSurface		= NULL;
		m_pDestTexture		= NULL;
		m_pSrcTexture		= NULL;
		m_bRenderTarget		= false;
		m_bCompress			= false;
		m_bKeepSrcTexture	= false;
		m_nWidth			= 0;
		m_nHeight			= 0;
		m_nTWidth			= 0;
		m_nTHeight			= 0;
	}
}

// テクスチャをロックして、ポインタを得る
bool STDCALLMETHOD CD3DTexture::LockTexture( D3DLOCKED_RECT* pLockRect, bool bKeepSrcTexture )
{
	if( m_bCreateTex && !m_bLocked ){
		// VideoRAMに存在するテクスチャは直接ロックすることができないので
		// システムメモリにテクスチャを作成し、それをロックして渡す
		//
		// m_bKeepSrcTextureフラグによって前回作成されたシステムテクスチャが維持されている場合は
		// この処理を飛ばして高速化を図る
		if( m_pSrcTexture == NULL ){
			IDirect3DDevice9* pD3DDevice;
			pD3DDevice = static_cast< IDirect3DDevice9* >( m_pD3DHelper->GetDevice() );
			if( pD3DDevice == NULL ){ return false; }
			// テクスチャで使用するフォーマットを判別
			D3DFORMAT ColorDepth;
			if( m_bRenderTarget || !m_bCompress ){
				ColorDepth = D3DFMT_A8R8G8B8;
			}else{
				ColorDepth = D3DFMT_DXT5;
			}
			// CreateOffscreenPlainSurfaceでシステムメモリサーフェスを作成し
			// それを使用することでロックを実現することも可能だが
			// オフスクリーンプレーンサーフェスで「D3DFMT_DXT5」フォーマットをサポートしないドライバが存在するため
			// (例：古いオンボードグラフィック等)
			// ここではシステムメモリテクスチャを作成し、ロック処理を行う
			IDirect3DTexture9* p;
			if( FAILED( pD3DDevice->CreateTexture( m_nTWidth, m_nTHeight, 1, 0, ColorDepth, D3DPOOL_SYSTEMMEM, &p, NULL ) ) ){
				return false;
			}
			m_pSrcTexture.Attach( p );
		}
		// GetRenderTargetDataメソッドを用いて
		// VideoRAMの内容をシステムテクスチャーにコピーする
		// ※GetRenderTargetDataメソッドはバックバッファや、「D3DUSAGE_RENDERTARGET」フラグつきテクスチャといった
		// 　いわゆるレンダリングターゲットしか転送元として受け付けないので
		// 　テクスチャ作成時は必ず、dwUsageに「D3DUSAGE_RENDERTARGET」フラグをセットすること
		// 　(つまりレンダーターゲットに指定できないD3DFMT_DXT5にはコピーできない)
		// 　また、転送先はメモリ管理オプションにD3DPOOL_SYSTEMMEMを指定しシステムメモリに確保したサーフェイスで
		// 　フォーマットとサイズが一致している必要がある
		//
		// 書き込み処理しか行わない場合は(レンダリングターゲットではない)
		// この処理を飛ばすことで高速化を行う
		if( m_bRenderTarget ){
			IDirect3DDevice9* pD3DDevice;
			pD3DDevice = static_cast< IDirect3DDevice9* >( m_pD3DHelper->GetDevice() );
			if( pD3DDevice == NULL ){ return false; }
			IDirect3DSurface9* p;
			m_pDestTexture->GetSurfaceLevel( 0, &p );
			CComPtr< IDirect3DSurface9 > pDestSurface;
			pDestSurface.Attach( p );
			m_pSrcTexture->GetSurfaceLevel( 0, &p );
			CComPtr< IDirect3DSurface9 > pSourceSurface;
			pSourceSurface.Attach( p );
			if( FAILED( pD3DDevice->GetRenderTargetData( pDestSurface, pSourceSurface ) ) ){
				return false;
			}
		}
		if( FAILED( m_pSrcTexture->LockRect( 0, reinterpret_cast< ::D3DLOCKED_RECT* >( pLockRect ), NULL, 0 ) ) ){
			return false;
		}
		// システムテクスチャ維持フラグをセット
		m_bKeepSrcTexture = bKeepSrcTexture;
		m_bLocked = true;
		return true;
	}
	return false;
}

// ロックされたテクスチャを解除する
void STDCALLMETHOD CD3DTexture::UnlockTexture( void )
{
	if( m_bCreateTex && m_bLocked ){
		// テクスチャをアンロック
		// アンロック後に内容を転送する
		// ただし取得モードの場合は対象外
		m_pSrcTexture->UnlockRect( 0 );
		{
			IDirect3DDevice9* pD3DDevice;
			pD3DDevice = static_cast< IDirect3DDevice9* >( m_pD3DHelper->GetDevice() );
			if( pD3DDevice == NULL ){ return; }
			//
			IDirect3DSurface9* p;
			m_pDestTexture->GetSurfaceLevel( 0, &p );
			CComPtr< IDirect3DSurface9 > pDestSurface;
			pDestSurface.Attach( p );
			m_pSrcTexture->GetSurfaceLevel( 0, &p );
			CComPtr< IDirect3DSurface9 > pSrcSurface;
			pSrcSurface.Attach( p );
			pD3DDevice->UpdateSurface( pSrcSurface, NULL, pDestSurface, NULL );
		}
		// サーフェスを残さないオプションの場合は
		// 不要になったサーフェスを解放
		if( !m_bKeepSrcTexture ){
			m_pSrcTexture = NULL;
		}
		m_bLocked = false;
	}
}

// テクスチャを一時的に削除
void CD3DTexture::Delete( void )
{
	if( m_bSetParam ){
		m_bCreateTex = false;
		m_pDepthSurface = NULL;
		m_pDestTexture = NULL;
	}
}

// テクスチャを作り直す
bool CD3DTexture::Recreate( void )
{
	if( m_bSetParam ){
		IDirect3DDevice9* pD3DDevice;
		pD3DDevice = static_cast< IDirect3DDevice9* >( m_pD3DHelper->GetDevice() );
		if( pD3DDevice == NULL ){ return false; }
		// テクスチャのフォーマット設定
		D3DFORMAT ColorDepth;
		DWORD dwUsage;
		if( m_bRenderTarget ){
			ColorDepth = D3DFMT_A8R8G8B8;
			dwUsage = D3DUSAGE_RENDERTARGET;
		}else if( !m_bCompress ){
			ColorDepth = D3DFMT_A8R8G8B8;
			dwUsage = 0;
		}else{
			ColorDepth = D3DFMT_DXT5;
			dwUsage = 0;
		}
		// テクスチャの再生成
		{
			IDirect3DTexture9* p;
			if( FAILED( pD3DDevice->CreateTexture( m_nTWidth, m_nTHeight, 1, dwUsage, ColorDepth, D3DPOOL_DEFAULT, &p, NULL ) ) ){
				return false;
			}
			m_pDestTexture.Attach( p );
		}
		// レンダリングターゲットの場合
		// ステンシルサーフェスも作成する
		if( m_bRenderTarget ){
			IDirect3DSurface9* p;
			if( FAILED( pD3DDevice->CreateDepthStencilSurface(
				m_nTWidth,
				m_nTHeight,
				D3DFMT_D16,
				D3DMULTISAMPLE_NONE,
				0,
				true,
				&p,
				NULL
				) ) ){
				return false;
			}
			m_pDepthSurface.Attach( p );
		}
		// 再初期化完了
		m_bCreateTex = true;
		// ゴミが表示されるのを防止するため
		// テクスチャを黒く塗りつぶす^^;
		{
			D3DLOCKED_RECT LockRect;
			if( LockTexture( &LockRect, false ) ){
				if( !m_bCompress ){
					::ZeroMemory( LockRect.pBits, LockRect.Pitch * m_nTHeight );
				}else{
					::ZeroMemory( LockRect.pBits, LockRect.Pitch * ( m_nTHeight / 4 ) );
				}
				UnlockTexture();
			}
		}
	}
	return true;
}
