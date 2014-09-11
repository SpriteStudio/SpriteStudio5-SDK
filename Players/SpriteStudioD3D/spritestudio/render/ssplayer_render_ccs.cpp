#include "stdafx.h"
#include <stdio.h>
#include <cstdlib>

#define DLL_EXPORT 

#pragma warning( push )
#pragma warning( disable : 4244 )// データが失われる可能性があります。

#include <D3D9.h>
#include <D3DX9.h>
#include "../Helper/ICCSInterface.h"
#include "../Helper/Direct3D.h"
#include "../Helper/CGlobalObject.h"
#include "../Helper/CGWindowBase.h"
#include "../Helper/CD3DTexture.h"
#include "../Helper/CD3DRender.h"
#include "../animator/ssplayer_animedecode.h"
#include "../animator/ssplayer_matrix.h"
#include "../helper/DebugPrint.h"
#include "ssplayer_render_ccs.h"
#include "ssplayer_texture_ccs.h"

#pragma warning( pop )

#define __PI__	(3.14159265358979323846f)
#define DegreeToRadian(Degree) ((double)Degree * (__PI__ / 180.0f))

inline float blendColorValue_(SsBlendType::_enum type, u8 color8, float rate)
{
	float c = static_cast<float>(color8) / 255.f;
	return c;

	switch (type)
	{
	case SsBlendType::mix:
		// GPU側で補間させるので何もしない。
		break;
	case SsBlendType::mul:
		// カラー値→１(テクスチャの色)に向けて補間する。
		c = c * rate + 1 * (1 - rate);
		break;
	case SsBlendType::add:
	case SsBlendType::sub:
		// c はテクスチャカラーに対してのオフセットになるため単純に率をかければいい。
		c = c * rate;
		break;
	}

	return c;
}

inline void blendColor_(float * dest, SsBlendType::_enum blendType, const SsColorBlendValue& color)
{
	const SsColor * srcColor = &color.rgba;
	float rate = color.rate;

	dest[0] = blendColorValue_(blendType, srcColor->r, rate);
	dest[1] = blendColorValue_(blendType, srcColor->g, rate);
	dest[2] = blendColorValue_(blendType, srcColor->b, rate);
}

void Transform( D3DFOURPARAM* pFourParam, const float* afMatrix )
{
	for( int i = 0 ; i < 4 ; i++ ){
		// 各座標の計算を行う
		float x = pFourParam->fDestX[ i ] * afMatrix[ 0 ] + pFourParam->fDestY[ i ] * afMatrix[ 4 ] + pFourParam->fDestZ[ i ] * afMatrix[ 8 ] + 1 * afMatrix[ 12 ];
		float y = pFourParam->fDestX[ i ] * afMatrix[ 1 ] + pFourParam->fDestY[ i ] * afMatrix[ 5 ] + pFourParam->fDestZ[ i ] * afMatrix[ 9 ] + 1 * afMatrix[ 13 ];
		float z = pFourParam->fDestX[ i ] * afMatrix[ 2 ] + pFourParam->fDestY[ i ] * afMatrix[ 6 ] + pFourParam->fDestZ[ i ] * afMatrix[ 10 ] + 1 * afMatrix[ 14 ];
		// 計算結果を実際に反映
		pFourParam->fDestX[ i ] = x;
		pFourParam->fDestY[ i ] = y;
		pFourParam->fDestZ[ i ] = z;
	}
}

void SsRenderCCS::changeAnimation( int packIndex, int animeIndex, SsAnimeDecoder* player, SsCellMapList* cellmap, SsProject* proj )
{
	//アニメパックを選択
	SsAnimePack* animepack = proj->getAnimePackList()[ packIndex ];

	//アニメパックのパーツ構造を取得
	SsModel* model = &animepack->Model;

	//アニメパック内のアニメーションを選択
	SsAnimation* anime = animepack->animeList[ animeIndex ];

	//セルマップ情報を作成
	cellmap->set( proj, animepack );

	//パーツ構造　アニメーション　セルマップからアニメ再生情報を作成する
	if ( cellmap->size() == 0 ){
		DEBUG_PRINTF( "error : cellmap array size == 0" );
		abort();
	}
	player->setAnimation( model, anime, cellmap, proj );
}

void SsRenderCCS::initialize()
{
	m_drawer.Attach( new CD3DRender( m_pD3DHelper ) );
	m_drawer->SetRenderParameter( D3DRENDER_MUL, D3DBLEND_NML, true, true, true );
	m_drawer->SetDrawNum( 1 );
	float matrix[ 16 ] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
	for( int i = 0; i < 16; i++ ){
		m_localmatrix[ i ] = matrix[ i ];
	}
	m_drawcount = 0;
	m_blendtype = SsBlendType::invalid;
}

void SsRenderCCS::renderSetup()
{
}

void SsRenderCCS::renderPart( SsPartState* state )
{
	D3DFOURPARAM FourParam = {
		{ 0, 0, 0, 0 },// SrcX
		{ 0, 0, 0, 0 },// SrcY
		{ 0, 0, 0, 0 },// DestX
		{ 0, 0, 0, 0 },// DestY
		{ 0.0f, 0.0f, 0.0f, 0.0f },// Z
		{ 1.0f, 1.0f, 1.0f, 1.0f },// RHW
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }// Color
	};

	if( state->hide ){ return; }

	// 頂点設定
	float* vertices = state->vertices;
	FourParam.fDestX[ 0 ] = vertices[ 6 ];
	FourParam.fDestY[ 0 ] = vertices[ 7 ];
	FourParam.fDestX[ 1 ] = vertices[ 9 ];
	FourParam.fDestY[ 1 ] = vertices[ 10 ];
	FourParam.fDestX[ 2 ] = vertices[ 0 ];
	FourParam.fDestY[ 2 ] = vertices[ 1 ];
	FourParam.fDestX[ 3 ] = vertices[ 3 ];
	FourParam.fDestY[ 3 ] = vertices[ 4 ];

	// UV設定
	SsVector2* uvs = state->cellValue.uvs;
	int w = 0;
	int h = 0;
	if( state->cellValue.texture != NULL ){
		w = state->cellValue.texture ->getWidth();
		h = state->cellValue.texture ->getHeight();
	}
	if( !state->hFlip ){
		if( !state->vFlip ){
			FourParam.fSrcX[ 0 ] = uvs[ 0 ].x * w;
			FourParam.fSrcY[ 0 ] = uvs[ 0 ].y * h;
			FourParam.fSrcX[ 1 ] = uvs[ 1 ].x * w;
			FourParam.fSrcY[ 1 ] = uvs[ 1 ].y * h;
			FourParam.fSrcX[ 2 ] = uvs[ 2 ].x * w;
			FourParam.fSrcY[ 2 ] = uvs[ 2 ].y * h;
			FourParam.fSrcX[ 3 ] = uvs[ 3 ].x * w;
			FourParam.fSrcY[ 3 ] = uvs[ 3 ].y * h;
		}else{
			FourParam.fSrcX[ 0 ] = uvs[ 2 ].x * w;
			FourParam.fSrcY[ 0 ] = uvs[ 2 ].y * h;
			FourParam.fSrcX[ 1 ] = uvs[ 3 ].x * w;
			FourParam.fSrcY[ 1 ] = uvs[ 3 ].y * h;
			FourParam.fSrcX[ 2 ] = uvs[ 0 ].x * w;
			FourParam.fSrcY[ 2 ] = uvs[ 0 ].y * h;
			FourParam.fSrcX[ 3 ] = uvs[ 1 ].x * w;
			FourParam.fSrcY[ 3 ] = uvs[ 1 ].y * h;
		}
	}else{
		if( !state->vFlip ){
			FourParam.fSrcX[ 0 ] = uvs[ 1 ].x * w;
			FourParam.fSrcY[ 0 ] = uvs[ 1 ].y * h;
			FourParam.fSrcX[ 1 ] = uvs[ 0 ].x * w;
			FourParam.fSrcY[ 1 ] = uvs[ 0 ].y * h;
			FourParam.fSrcX[ 2 ] = uvs[ 3 ].x * w;
			FourParam.fSrcY[ 2 ] = uvs[ 3 ].y * h;
			FourParam.fSrcX[ 3 ] = uvs[ 2 ].x * w;
			FourParam.fSrcY[ 3 ] = uvs[ 2 ].y * h;
		}else{
			FourParam.fSrcX[ 0 ] = uvs[ 3 ].x * w;
			FourParam.fSrcY[ 0 ] = uvs[ 3 ].y * h;
			FourParam.fSrcX[ 1 ] = uvs[ 2 ].x * w;
			FourParam.fSrcY[ 1 ] = uvs[ 2 ].y * h;
			FourParam.fSrcX[ 2 ] = uvs[ 1 ].x * w;
			FourParam.fSrcY[ 2 ] = uvs[ 1 ].y * h;
			FourParam.fSrcX[ 3 ] = uvs[ 0 ].x * w;
			FourParam.fSrcY[ 3 ] = uvs[ 0 ].y * h;
		}
	}

	// 行列適用
	Transform( &FourParam, state->matrix );

	// 最終スケール・最終オフセットを適用
	Transform( &FourParam, m_localmatrix );

	// アルファチャンネル
	DWORD alpha = static_cast< DWORD >( 255 * state->alpha );
	DWORD alphas[ 4 ] = { alpha, alpha, alpha, alpha };
	// カラーブレンド
	DWORD color;
	DWORD colors[ 4 ] = { 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF };
	if( state->is_color_blend ){
		// 頂点カラーがある時だけブレンド計算する
		if( state->colorValue.target == SsColorBlendTarget::whole ){
			// 単色
			const SsColorBlendValue& cbv = state->colorValue.color;
			// α
			alphas[ 0 ] = static_cast< DWORD >( alphas[ 0 ] * cbv.rgba.a / 255.0f ) << 24;
			alphas[ 1 ] = static_cast< DWORD >( alphas[ 1 ] * cbv.rgba.a / 255.0f ) << 24;
			alphas[ 2 ] = static_cast< DWORD >( alphas[ 2 ] * cbv.rgba.a / 255.0f ) << 24;
			alphas[ 3 ] = static_cast< DWORD >( alphas[ 3 ] * cbv.rgba.a / 255.0f ) << 24;
			// RGB
			color = cbv.rgba.b | ( cbv.rgba.g << 8 ) | ( cbv.rgba.r << 16 );
			colors[ 0 ] = color;
			colors[ 1 ] = color;
			colors[ 2 ] = color;
			colors[ 3 ] = color;
		}else{
			for( int i = 0; i < 4; i++ ){
				// 各頂点カラー
				const SsColorBlendValue& cbv = state->colorValue.colors[ i ];
				// α
				alphas[ i ] = static_cast< DWORD >( alphas[ i ] * cbv.rgba.a / 255.0f ) << 24;
				// RGB
				color = cbv.rgba.b | ( cbv.rgba.g << 8 ) | ( cbv.rgba.r << 16 );
				colors[ i ] = color;
			}
		}
	}else{
		alphas[ 0 ] = alphas[ 0 ] << 24;
		alphas[ 1 ] = alphas[ 1 ] << 24;
		alphas[ 2 ] = alphas[ 2 ] << 24;
		alphas[ 3 ] = alphas[ 3 ] << 24;
	}
	FourParam.Color[ 0 ] = alphas[ 0 ] | colors[ 0 ];
	FourParam.Color[ 1 ] = alphas[ 1 ] | colors[ 1 ];
	FourParam.Color[ 2 ] = alphas[ 2 ] | colors[ 2 ];
	FourParam.Color[ 3 ] = alphas[ 3 ] | colors[ 3 ];

	// 合成方法
	// 変更があった場合はレンダリングを行う
	if( m_blendtype != state->alphaBlendType ){
		m_drawer->Rendering();
		switch( state->alphaBlendType ){
		case SsBlendType::mix:
			m_drawer->SetRenderParameter( D3DRENDER_MUL, D3DBLEND_NML, true, true, true );
			break;
		case SsBlendType::mul:
			m_drawer->SetRenderParameter( D3DRENDER_MUL, D3DBLEND_MUL, true, true, true );
			break;
		case SsBlendType::add:
			m_drawer->SetRenderParameter( D3DRENDER_MUL, D3DBLEND_ADD, true, true, true );
			break;
		case SsBlendType::sub:
			m_drawer->SetRenderParameter( D3DRENDER_MUL, D3DBLEND_SUB, true, true, true );
			break;
		}
		m_blendtype = state->alphaBlendType;
	}

	// テクスチャ
	SSTextureCCS* drawTex = static_cast< SSTextureCCS* >( state->cellValue.texture );
	if( drawTex != NULL && m_drawer->GetTexture() != drawTex->getTexture() ){
		m_drawer->SetTexture( drawTex->getTexture() );
	}

	// 描画
	m_drawer->Draw( &FourParam );
	m_drawer->Rendering();

	m_drawcount++;
}
