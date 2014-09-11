/*
 * @概要		レンダリングクラス
 *
 * $Id: 79dad66ddacd304b9c13286923e243a47d44eb59 $
 *
 * @author		諒
 * @since		2002/05/05
 */

#include "Stdafx.h"
#include <D3D9.h>
#include <DDraw.h>
#include "CBaseD3DRender.h"

using namespace std;

// レンダリングモードの指定
void CBaseD3DRender::SetRendering( void )
{
	// 色の算出方法の設定
	switch( m_RenderType ){
	case D3DRENDER_MUL:
		// 乗算
		m_pD3DHelper->SetTexColorOp( D3DTOP_MODULATE );
		m_pD3DHelper->SetTexColorArg1( D3DTA_TEXTURE );
		m_pD3DHelper->SetTexColorArg2( D3DTA_DIFFUSE );
		// αの算出方法の設定
		m_pD3DHelper->SetTexAlphaOp( D3DTOP_MODULATE );
		m_pD3DHelper->SetTexAlphaArg1( D3DTA_TEXTURE );
		m_pD3DHelper->SetTexAlphaArg2( D3DTA_DIFFUSE );
		break;
	case D3DRENDER_ADD:
		// 加算
		m_pD3DHelper->SetTexColorOp( D3DTOP_ADD );
		m_pD3DHelper->SetTexColorArg1( D3DTA_TEXTURE );
		m_pD3DHelper->SetTexColorArg2( D3DTA_DIFFUSE );
		// αの算出方法の設定
		m_pD3DHelper->SetTexAlphaOp( D3DTOP_ADD );
		m_pD3DHelper->SetTexAlphaArg1( D3DTA_TEXTURE );
		m_pD3DHelper->SetTexAlphaArg2( D3DTA_DIFFUSE );
		break;
	case D3DRENDER_DIF:
		// ディフューズ色使用
		m_pD3DHelper->SetTexColorOp( D3DTOP_SELECTARG1 );
		m_pD3DHelper->SetTexColorArg1( D3DTA_DIFFUSE );
		m_pD3DHelper->SetTexColorArg2( D3DTA_DIFFUSE );
		// αの算出方法の設定
		m_pD3DHelper->SetTexAlphaOp( D3DTOP_SELECTARG1 );
		m_pD3DHelper->SetTexAlphaArg1( D3DTA_DIFFUSE );
		m_pD3DHelper->SetTexAlphaArg2( D3DTA_DIFFUSE );
		break;
	}

	// αブレンドの方法設定
	if( m_bAlphaBlend ){
		// 合成後の色の算出方法の設定
		switch( m_BlendType ){
		case D3DBLEND_NML:
			// 通常ブレンド
			m_pD3DHelper->SetBlendOp( D3DBLENDOP_ADD );
			m_pD3DHelper->SetSrcBlend( D3DBLEND_SRCALPHA );
			m_pD3DHelper->SetDstBlend( D3DBLEND_INVSRCALPHA );
			// 合成後のα値の算出方法の設定
			m_pD3DHelper->SetBlendOpAlpha( D3DBLENDOP_MAX );
			m_pD3DHelper->SetSrcBlendAlpha( D3DBLEND_ONE );
			m_pD3DHelper->SetDstBlendAlpha( D3DBLEND_ONE );
			break;
		case D3DBLEND_ADD:
			// 加算
			m_pD3DHelper->SetBlendOp( D3DBLENDOP_ADD );
			m_pD3DHelper->SetSrcBlend( D3DBLEND_SRCALPHA );
			m_pD3DHelper->SetDstBlend( D3DBLEND_ONE );
			// 合成後のα値の算出方法の設定
			m_pD3DHelper->SetBlendOpAlpha( D3DBLENDOP_ADD );
			m_pD3DHelper->SetSrcBlendAlpha( D3DBLEND_ONE );
			m_pD3DHelper->SetDstBlendAlpha( D3DBLEND_ONE );
			break;
		case D3DBLEND_SUB:
			// 減算
			m_pD3DHelper->SetBlendOp( D3DBLENDOP_REVSUBTRACT );
			m_pD3DHelper->SetSrcBlend( D3DBLEND_SRCALPHA );
			m_pD3DHelper->SetDstBlend( D3DBLEND_ONE );
			// 合成後のα値の算出方法の設定
			m_pD3DHelper->SetBlendOpAlpha( D3DBLENDOP_REVSUBTRACT );
			m_pD3DHelper->SetSrcBlendAlpha( D3DBLEND_ONE );
			m_pD3DHelper->SetDstBlendAlpha( D3DBLEND_ONE );
			break;
		case D3DBLEND_MUL:
			// 乗算
			m_pD3DHelper->SetBlendOp( D3DBLENDOP_ADD );
			m_pD3DHelper->SetSrcBlend( D3DBLEND_ZERO );
			m_pD3DHelper->SetDstBlend( D3DBLEND_SRCCOLOR );
			// 合成後のα値の算出方法の設定
			m_pD3DHelper->SetBlendOpAlpha( D3DBLENDOP_ADD );
			m_pD3DHelper->SetSrcBlendAlpha( D3DBLEND_ZERO );
			m_pD3DHelper->SetDstBlendAlpha( D3DBLEND_SRCALPHA );
			break;
		case D3DBLEND_OVR:
			// 上書き転送
			m_pD3DHelper->SetBlendOp( D3DBLENDOP_ADD );
			m_pD3DHelper->SetSrcBlend( D3DBLEND_ONE );
			m_pD3DHelper->SetDstBlend( D3DBLEND_ZERO );
			// 合成後のα値の算出方法の設定
			m_pD3DHelper->SetBlendOpAlpha( D3DBLENDOP_ADD );
			m_pD3DHelper->SetSrcBlendAlpha( D3DBLEND_ONE );
			m_pD3DHelper->SetDstBlendAlpha( D3DBLEND_ZERO );
			break;
		case D3DBLEND_CHI:
			// 色のみ加算
			m_pD3DHelper->SetBlendOp( D3DBLENDOP_ADD );
			m_pD3DHelper->SetSrcBlend( D3DBLEND_ONE );
			m_pD3DHelper->SetDstBlend( D3DBLEND_ONE );
			// 合成後のα値の算出方法の設定
			m_pD3DHelper->SetBlendOpAlpha( D3DBLENDOP_ADD );
			m_pD3DHelper->SetSrcBlendAlpha( D3DBLEND_ZERO );
			m_pD3DHelper->SetDstBlendAlpha( D3DBLEND_ONE );
			break;
		case D3DBLEND_CHD:
			// 色のみ減算
			m_pD3DHelper->SetBlendOp( D3DBLENDOP_REVSUBTRACT );
			m_pD3DHelper->SetSrcBlend( D3DBLEND_ONE );
			m_pD3DHelper->SetDstBlend( D3DBLEND_ONE );
			// 合成後のα値の算出方法の設定
			m_pD3DHelper->SetBlendOpAlpha( D3DBLENDOP_ADD );
			m_pD3DHelper->SetSrcBlendAlpha( D3DBLEND_ZERO );
			m_pD3DHelper->SetDstBlendAlpha( D3DBLEND_ONE );
			break;
		case D3DBLEND_CHM:
			// 色のみ乗算
			m_pD3DHelper->SetBlendOp( D3DBLENDOP_ADD );
			m_pD3DHelper->SetSrcBlend( D3DBLEND_ZERO );
			m_pD3DHelper->SetDstBlend( D3DBLEND_SRCCOLOR );
			// 合成後のα値の算出方法の設定
			m_pD3DHelper->SetBlendOpAlpha( D3DBLENDOP_ADD );
			m_pD3DHelper->SetSrcBlendAlpha( D3DBLEND_ZERO );
			m_pD3DHelper->SetDstBlendAlpha( D3DBLEND_ONE );
			break;
		case D3DBLEND_ACI:
			// αのみ加算
			m_pD3DHelper->SetBlendOp( D3DBLENDOP_ADD );
			m_pD3DHelper->SetSrcBlend( D3DBLEND_ZERO );
			m_pD3DHelper->SetDstBlend( D3DBLEND_ONE );
			// 合成後のα値の算出方法の設定
			m_pD3DHelper->SetBlendOpAlpha( D3DBLENDOP_ADD );
			m_pD3DHelper->SetSrcBlendAlpha( D3DBLEND_ONE );
			m_pD3DHelper->SetDstBlendAlpha( D3DBLEND_ONE );
			break;
		case D3DBLEND_ACD:
			// αのみ減算
			m_pD3DHelper->SetBlendOp( D3DBLENDOP_ADD );
			m_pD3DHelper->SetSrcBlend( D3DBLEND_ZERO );
			m_pD3DHelper->SetDstBlend( D3DBLEND_ONE );
			// 合成後のα値の算出方法の設定
			m_pD3DHelper->SetBlendOpAlpha( D3DBLENDOP_REVSUBTRACT );
			m_pD3DHelper->SetSrcBlendAlpha( D3DBLEND_ONE );
			m_pD3DHelper->SetDstBlendAlpha( D3DBLEND_ONE );
		case D3DBLEND_ACM:
			// αのみ乗算
			m_pD3DHelper->SetBlendOp( D3DBLENDOP_ADD );
			m_pD3DHelper->SetSrcBlend( D3DBLEND_ZERO );
			m_pD3DHelper->SetDstBlend( D3DBLEND_ONE );
			// 合成後のα値の算出方法の設定
			m_pD3DHelper->SetBlendOpAlpha( D3DBLENDOP_ADD );
			m_pD3DHelper->SetSrcBlendAlpha( D3DBLEND_ZERO );
			m_pD3DHelper->SetDstBlendAlpha( D3DBLEND_SRCALPHA );
		}
		// αブレンドを有効にする
		m_pD3DHelper->SetAlphaBlendEnable( TRUE );
		m_pD3DHelper->SetSeparateAlphaBlendEnable( TRUE );
	}else{
		// αブレンドは無効
		m_pD3DHelper->SetAlphaBlendEnable( FALSE );
		m_pD3DHelper->SetSeparateAlphaBlendEnable( FALSE );
	}
	if( m_bAlphaTest ){
		// αテストを有効にする
		m_pD3DHelper->SetAlphaTestEnable( TRUE );
		m_pD3DHelper->SetAlphaRef( 0x01 );
		m_pD3DHelper->SetAlphaFunc( D3DCMP_GREATEREQUAL );
	}else{
		// αテストは無効
		m_pD3DHelper->SetAlphaTestEnable( FALSE );
	}
	if( m_bLinearFilter ){
		// リニアフィルターを有効にする
		m_pD3DHelper->SetScaleState( D3DTEXF_LINEAR );
	}else{
		// リニアフィルターは無効
		m_pD3DHelper->SetScaleState( D3DTEXF_POINT );
	}
}

// テクスチャを変更
bool CBaseD3DRender::SetTexture( CD3DTexture* pTexture )
{
	if( pTexture != NULL && pTexture->IsInited() ){
		// アスペクト比を取得
		m_fAspectX = 1 / static_cast< float >( pTexture->GetTWidth() );
		m_fAspectY = 1 / static_cast< float >( pTexture->GetTHeight() );
		// テクスチャ割り当て
		m_pTexture = pTexture;
		return true;
	}
	return false;
}
