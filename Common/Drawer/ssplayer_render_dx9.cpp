#include <stdio.h>
#include <cstdlib>



#include "../helper/DirectX/SSTextureDX9.h"
#include "ssplayer_animedecode.h"
#include "ssplayer_render_dx9.h"
#include "ssplayer_matrix.h"
//#include "ssplayer_shader_gl.h"


enum{
	PG_SHADER_NPOT,
	PG_SHADER_POT,
};




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


void	SsRenderDX9::initialize()
{
#if 0
//	if ( m_isInit ) return ;
	SSOpenGLShaderMan::Create();
	SSOpenGLVertexShader*	vs = new SSOpenGLVertexShader( "basic_vs" , glshader_sprite_vs );
	SSOpenGLFragmentShader*	fs1 = new SSOpenGLFragmentShader( "normal_fs" , glshader_sprite_fs );
	SSOpenGLFragmentShader*	fs2 = new SSOpenGLFragmentShader( "pot_fs" ,glshader_sprite_fs_pot );

	SSOpenGLProgramObject* pgo1 = new SSOpenGLProgramObject();
	SSOpenGLProgramObject* pgo2 = new SSOpenGLProgramObject();

	pgo1->Attach( vs );
	pgo1->Attach( fs1 );
	pgo1->Link();
	SSOpenGLShaderMan::PushPgObject( pgo1 );

	pgo2->Attach( vs );
	pgo2->Attach( fs2 );
	pgo2->Link();
	SSOpenGLShaderMan::PushPgObject( pgo2 );

//	m_isInit = true;
#endif
}

void	SsRenderDX9::renderSetup()
{
#if 0
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
#endif
}


void	SsRenderDX9::renderPart( SsPartState* state )
{

#if 0
	bool texture_is_pow2 = true;
	bool color_blend_v4 = false;
	float vertexID[10];
	bool  colorBlendEnabled = false;
	bool  alphaBlendMix = false;

	int		gl_target = GL_TEXTURE_2D;
	float	rates[5];


	if ( state->hide ) return ;


	SsCell * cell = state->cellValue.cell;
	if ( cell == 0 ) return ;


	//int		cellimage = state->cellValue.texture;
	ISSTexture*	texture = state->cellValue.texture;


	if ( state->cellValue.cellmapl == 0  ) return ;

	SsCellMap* map = state->cellValue.cellmapl->cellMap;

	if (cell)
	{
		for ( int i = 0 ; i < 5 ; i++ ) rates[i] = 0.0f;
	}else{
		for ( int i = 0 ; i < 5 ; i++ ) rates[i] = 1.0f;
	}

	if (cell)
	{
		// テクスチャのサイズが2のべき乗かチェック
		if (SsUtTextureisPow2(map->pixelSize.x) &&
			SsUtTextureisPow2(map->pixelSize.y))
		{
			// 2のべき乗
			texture_is_pow2 = true;
			gl_target = GL_TEXTURE_2D;
		}
		else
		{
			// 2のべき乗ではない:NPOTテクスチャ
			texture_is_pow2 = false;
			gl_target = GL_TEXTURE_RECTANGLE_ARB;
		}


		glEnable(gl_target);

#if PROGRAMABLE_SHADER_ON
		if ( glpgObject )
		{
			glActiveTexture(GL_TEXTURE0);
		}
#endif

		SSTextureGL* tex_gl = (SSTextureGL*)texture;
		glBindTexture(gl_target, tex_gl->tex);


		// フィルタ
		GLint filterMode;

		SsTexFilterMode::_enum fmode = state->cellValue.cellmapl->cellMap->filterMode;
		SsTexWrapMode::_enum wmode = state->cellValue.cellmapl->cellMap->wrapMode;

		switch (fmode)
		{
		default:
		case SsTexFilterMode::nearest:
			filterMode = GL_NEAREST;
			break;
		case SsTexFilterMode::linear:
			filterMode = GL_LINEAR;
			break;
		}

		glTexParameteri(gl_target, GL_TEXTURE_MIN_FILTER, filterMode);
		glTexParameteri(gl_target, GL_TEXTURE_MAG_FILTER, filterMode);

		// ラップモード
		GLint wrapMode; 
		switch (wmode)
		{
		default:
		case SsTexWrapMode::clamp:
			wrapMode = GL_CLAMP_TO_EDGE;
			if (texture_is_pow2 == false)
			{
				wrapMode = GL_CLAMP;
			}
			break;
		case SsTexWrapMode::repeat:
			wrapMode = GL_REPEAT;	// TODO 要動作確認
			break;
		case SsTexWrapMode::mirror:
			wrapMode = GL_MIRRORED_REPEAT;	// TODO 要動作確認
			break;
		}

		glTexParameteri(gl_target, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(gl_target, GL_TEXTURE_WRAP_T, wrapMode);

		// セルが持つUV値をまず設定
		std::memcpy( state->uvs, state->cellValue.uvs, sizeof( state->uvs ));

		// UV アニメの適用
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();

		float uvw = state->cellValue.uvs[3].x + state->cellValue.uvs[0].x;
		float uvh = state->cellValue.uvs[3].y + state->cellValue.uvs[0].y;
        SsVector2 uv_trans;

		if (texture_is_pow2)
		{
			uv_trans.x = state->uvTranslate.x;
			uv_trans.y = state->uvTranslate.y;
		}
		else
		{
			uv_trans.x = state->uvTranslate.x * map->pixelSize.x;
			uv_trans.y = state->uvTranslate.y * map->pixelSize.y;

			//中心座標を計算
			uvw*= map->pixelSize.x;
			uvh*= map->pixelSize.y;
		}

		uvw/=2.0f;
		uvh/=2.0f;

		glTranslatef( uvw + uv_trans.x , uvh + uv_trans.y , 0 );
		glRotatef( state->uvRotation, 0.0, 0.0, 1.0);

		float	uvsh = state->uvScale.x;
		float	uvsv = state->uvScale.y;

		if ( state->imageFlipH ) uvsh*=-1;
		if ( state->imageFlipV ) uvsv*=-1;

		glScalef( uvsh , uvsv , 0.0);
		glTranslatef( -uvw , -uvh , 0 );

		glMatrixMode(GL_MODELVIEW);
		
		// オリジナルUVをまず指定する
		// 左下が 0,0 Ｚ字順
		static const int sUvOrders[][4] = {
			{0, 1, 2, 3},	// フリップなし
			{1, 0, 3, 2},	// 水平フリップ
			{2, 3, 0, 1},	// 垂直フリップ
			{3, 2, 1, 0},	// 両方フリップ
		};

    	//memset( state->uvs , 0 , sizeof(float) * 10 );

		// イメージのみのフリップ対応

		int order = ( state->hFlip == true ? 1 : 0 ) + ( state->vFlip == true ? 1 :0 ) * 2;
		float	uvs[10];
		const int * uvorder = &sUvOrders[order][0];
		for (int i = 0; i < 4; ++i)
		{
			int idx = *uvorder;
			if (texture_is_pow2 == false)
			{
				// GL_TEXTURE_RECTANGLE_ARBではuv座標系が0～1ではなくピクセルになるので変換
				uvs[idx * 2] = state->cellValue.uvs[i].x * map->pixelSize.x;
				uvs[idx * 2 + 1] = state->cellValue.uvs[i].y * map->pixelSize.y;

#if USE_TRIANGLE_FIN
				//きれいな頂点変形への対応
				uvs[4 * 2] += state->uvs[idx * 2];
				uvs[4 * 2 + 1] += state->uvs[idx * 2 + 1];
#endif
			}
			else
			{
				uvs[idx * 2] = state->cellValue.uvs[i].x;
				uvs[idx * 2 + 1] = state->cellValue.uvs[i].y;

#if USE_TRIANGLE_FIN
				//きれいな頂点変形への対応
				uvs[4 * 2] += state->uvs[idx * 2];
				uvs[4 * 2 + 1] += state->uvs[idx * 2 + 1];
#endif
			}
			++uvorder;
		}

#if USE_TRIANGLE_FIN
		//きれいな頂点変形への対応
		uvs[4*2]/=4.0f;
		uvs[4*2+1]/=4.0f;

#endif

		// UV 配列を指定する
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid *)uvs);
	}else{
	   //セルがない状態というのは本当は有効な状態ではないような気がするがNANが入るので何かを入れる
	   for ( int i =0 ; i < 10 ; i ++ ) state->uvs[i] = 0;

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid *)state->uvs);
		//テクスチャのバインドを解除する　状況が不定になるため
        glBindTexture(gl_target, 0);
	}

	// αブレンド
	{

		glBlendEquation( GL_FUNC_ADD );

		// 演算方法の指定
		switch (state->alphaBlendType)
		{
		case SsBlendType::mix:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case SsBlendType::mul:
			// TODO SrcAlpha を透明度として使えない
			//glBlendFunc(GL_ZERO, GL_SRC_COLOR);
			glBlendFuncSeparateEXT( GL_ZERO, GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA, GL_ONE );
			break;
		case SsBlendType::add:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			//glBlendFuncSeparateEXT( GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE );

			break;
		case SsBlendType::sub:
			// TODO SrcAlpha を透明度として使えない
			glBlendEquation( GL_FUNC_REVERSE_SUBTRACT );
			glBlendFuncSeparateEXT( GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_DST_ALPHA );

			break;
		}
	}


#if 1
	// 頂点カラーの指定
	{
		//SsAttrKeyList *	keys = anime ? anime->keyframes(SsAttributeKind::color) : NULL;
		void* keys = 0;
		if (state->is_color_blend)
		{
			colorBlendEnabled = true;
			// 頂点カラーがある時だけブレンド計算する
			if ( state->colorValue.target == SsColorBlendTarget::whole)
			{
				// 単色
				const SsColorBlendValue& cbv = state->colorValue.color;
				// RGB値の事前ブレンド
				blendColor_( state->colors, state->colorValue.blendType, cbv);
				// α値のブレンド。常に乗算とする。
				state->colors[3] = blendColorValue_(SsBlendType::mul, cbv.rgba.a * state->alpha, cbv.rate);
				rates[0] = cbv.rate;
				vertexID[0] = 0;

				// 残り３つは先頭のをコピー
				for (int i = 1; i < 5; ++i)
				{
					memcpy( state->colors + i * 4, state->colors, sizeof(state->colors[0]) * 4);
					rates[i] = cbv.rate;
					vertexID[i*2] = 0;
					vertexID[i*2+1] = 0;
					//vertexID[i] = 0;
				}
			}
			else
			{

				color_blend_v4 = true;
				state->colors[4*4+0] = 0 ;
				state->colors[4*4+1] = 0 ;
				state->colors[4*4+2] = 0 ;
				state->colors[4*4+3] = 0 ;
				rates[4] = 0 ;


				// 頂点単位
				for (int i = 0; i < 4; ++i)
				{
					// RGB値の事前ブレンド
					const SsColorBlendValue& cbv = state->colorValue.colors[i];
					blendColor_( state->colors + i * 4, state->colorValue.blendType, cbv);
					// α値のブレンド。常に乗算とする。
					state->colors[i * 4 + 3] = blendColorValue_(SsBlendType::mul, cbv.rgba.a * state->alpha, cbv.rate);
					rates[i] = cbv.rate;
					vertexID[i*2] = i;
					vertexID[i*2+1] = i;
				}

#if USE_TRIANGLE_FIN
				float a,r,g,b ,rate;
				a = r = g = b = rate = 0;
				for  ( int i = 0 ; i < 4 ; i ++ )
				{
					a+=state->colors[i*4+0];
					r+=state->colors[i*4+1];
					g+=state->colors[i*4+2];
					b+=state->colors[i*4+3];
					rate+=rates[i];
				}



				//きれいな頂点変形への対応
				vertexID[4*2] = 4;
				vertexID[4*2+1] = 4;

				state->colors[4*4+0]=a/4.0f;
				state->colors[4*4+1]=r/4.0f;
				state->colors[4*4+2]=g/4.0f;
				state->colors[4*4+3]=b/4.0f;
				rates[4]= rate / 4.0f;
#endif

			}
		}
		else
		{
			for (int i = 0; i < 5; ++i)
				state->colors[i * 4 + 3] = state->alpha;

			// カラーは１００％テクスチャ
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE0);
			// αだけ合成
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_TEXTURE0);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, GL_PRIMARY_COLOR);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
		}


		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, 0, (GLvoid *)state->colors);
	}

#else
	for (int i = 0; i < 5; ++i)
		state->colors[i * 4 + 3] = state->alpha;

	// カラーは１００％テクスチャ
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE0);
	// αだけ合成
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_TEXTURE0);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, GL_PRIMARY_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, (GLvoid *)state->colors);

#endif



	// 頂点バッファの設定
	glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, (GLvoid *)state->vertices);

	glPushMatrix();

	// update で計算しておいた行列をロード
	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
	glLoadMatrixf(state->matrix);

	GLint VertexLocation;
//glpgObject = 0;
	if (state->noCells)
	{
		// TODO とりあえず当たり判定パーツなのでただポリゴンを描く
		// -> レイアウトビューへ持っていきました  kurooka
		//glDisable(GL_TEXTURE_2D);
		//glPopMatrix();
	}else{

#if PROGRAMABLE_SHADER_ON
	if ( colorBlendEnabled )
	{

		if ( GL_TEXTURE_RECTANGLE_ARB == gl_target )
		{
			SSOpenGLShaderMan::SetCurrent(PG_SHADER_NPOT);
		}else{
			SSOpenGLShaderMan::SetCurrent(PG_SHADER_POT);
		}

		if ( glpgObject )
		{
			VertexLocation = glpgObject->GetAttribLocation( "vertexID" );
			glVertexAttribPointer( VertexLocation , 2 , GL_FLOAT , GL_FALSE, 0, vertexID);//GL_FALSE→データを正規化しない
			glEnableVertexAttribArray(VertexLocation);//有効化

			//シェーダのセットアップ
			glpgObject->Enable();
			int type = (int)state->colorValue.blendType;

			//セレクター非０はきちんとチェックすること
			GLint uid = glpgObject->GetUniformLocation( "selector" );
			if ( uid >= 0 )
				glUniform1i( uid , type );


			uid = glpgObject->GetUniformLocation( "rates" );
			if ( uid >= 0 )
				glUniform1fv( uid , 5 , rates );
		}
	}

#endif

#if USE_TRIANGLE_FIN
			//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		if ( state->is_vertex_transform || state->is_color_blend )
		{
			static const GLubyte indices[] = { 4 , 3, 1, 0, 2 , 3};
			glDrawElements(GL_TRIANGLE_FAN, 6, GL_UNSIGNED_BYTE, indices);
//			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}else{
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		}
#else
		// 頂点配列を描画
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

#endif
	}

#if PROGRAMABLE_SHADER_ON
	if ( glpgObject )
	{
		if ( colorBlendEnabled )
		{
			if ( glpgObject )
			{
				glDisableVertexAttribArray(VertexLocation);//無効化
				glpgObject->Disable();
			}
		}
	}
#endif

	glPopMatrix();

	if (texture_is_pow2 == false)
	{
		glDisable(gl_target);
	}
	glDisable(GL_TEXTURE_2D);
	glBlendEquation( GL_FUNC_ADD );
#endif

}
