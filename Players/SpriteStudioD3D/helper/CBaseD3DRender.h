/*
 * @概要		レンダリングクラスの基底クラス
 *
 * $Id: b477ab39406ec973cdc77c122d61565cd7d51be0 $
 *
 * @author		ツブ
 * @since		2008/01/20
 */

#pragma once

#include "CD3DTexture.h"

// ======================================================================
//  クラス定義
// ======================================================================
class CBaseD3DRender
{
public:
	CBaseD3DRender::CBaseD3DRender( CDirect3DHelper* pD3DHelper ):
		m_RenderType( D3DRENDER_MUL ),
		m_pD3DHelper( pD3DHelper ),
		m_bAlphaBlend( false ),
		m_bAlphaTest( false ),
		m_bLinearFilter( false ),
		m_BlendType( D3DBLEND_NML ),
		m_fAspectX( 0.0f ), m_fAspectY( 0.0f ), m_fAspectZ( 1 / 65535.0f ){}

	CDirect3DHelper* GetD3DHelper( void ){ return m_pD3DHelper; }
	CD3DTexture* GetTexture( void ){ return m_pTexture; }

	// レンダリングモードの取得
	void GetRenderParameter( D3DRENDERTYPE* pRenderType, D3DBLENDTYPE* pBlendType = NULL, bool* pbAlphaBlend = NULL, bool* pbAlphaTest = NULL, bool* pbLinearFilter = NULL )const
	{
		if( pRenderType != NULL ){
			*pRenderType = m_RenderType;
		}
		if( pBlendType != NULL ){
			*pBlendType = m_BlendType;
		}
		if( pbAlphaBlend != NULL ){
			*pbAlphaBlend = m_bAlphaBlend;
		}
		if( pbAlphaTest != NULL ){
			*pbAlphaTest = m_bAlphaTest;
		}
		if( pbLinearFilter != NULL ){
			*pbLinearFilter = m_bLinearFilter;
		}
	}

	// レンダリングモードの設定
	void SetRenderParameter( D3DRENDERTYPE RenderType, D3DBLENDTYPE BlendType, bool bAlphaBlend, bool bAlphaTest, bool bLinearFilter )
	{
		if( RenderType < D3DRENDER_SIZE
				&& BlendType < D3DBLEND_SIZE ){
			m_RenderType	= RenderType;
			m_BlendType		= BlendType;
			m_bAlphaBlend	= bAlphaBlend;
			m_bAlphaTest	= bAlphaTest;
			m_bLinearFilter	= bLinearFilter;
		}
	}

	// レンダリングモードの指定
	void SetRendering( void );
	bool SetTexture( CD3DTexture* pTexture );

	//アスペクト取得
	float GetAspectX() const { return m_fAspectX; }
	float GetAspectY() const { return m_fAspectY; }
	float GetAspectZ() const { return m_fAspectZ; }
private:
	CComPtr< CDirect3DHelper >				m_pD3DHelper;
	CComPtr< CD3DTexture >					m_pTexture;
	D3DRENDERTYPE							m_RenderType;
	D3DBLENDTYPE							m_BlendType;
	bool									m_bAlphaBlend, m_bAlphaTest, m_bLinearFilter;
	float									m_fAspectX, m_fAspectY, m_fAspectZ;
};
