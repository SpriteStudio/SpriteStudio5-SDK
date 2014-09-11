#include "stdafx.h"
#include <stdio.h>
#include <cstdlib>

#pragma warning( push )
#pragma warning( disable: 4244 )//データが失われる可能性があります。
#pragma warning( disable: 4100 )//引数は関数の本体部で 1 度も参照されません。
#pragma warning( disable: 4189 )//ローカル変数が初期化されましたが、参照されていません
#pragma warning( disable: 4101 )//ローカル変数は 1 度も使われていません。
#pragma warning( disable: 4800 )//ブール値を 'true' または 'false' に強制的に設定します

#include "../loader/ssloader.h"
#include "ssplayer_animedecode.h"
#include "ssplayer_matrix.h"
#include "ssplayer_render.h"

#define __PI__	(3.14159265358979323846f)
#define RadianToDegree(Radian) ((double)Radian * (180.0f / __PI__))
#define DegreeToRadian(Degree) ((double)Degree * (__PI__ / 180.0f))

//stdでののforeach宣言　
#define USE_TRIANGLE_FIN (0)





SsAnimeDecoder::SsAnimeDecoder() : 
	curAnimeFPS(0),
	curAnimeEndFrame(0),
	nowPlatTime(0) ,
	curCellMapManager(0),
	partState(0)
	{
	}



void	SsAnimeDecoder::setAnimation( SsModel*	model , SsAnimation* anime , SsCellMapList* cellmap , SsProject* sspj )
{
	//セルマップリストを取得
	curCellMapManager = cellmap;
	curAnimation = anime;

	//partStateをパーツ分作成する
	partAnimeDic.clear();

	//パーツの数
	size_t panum = anime->partAnimes.size();
	for ( size_t i = 0 ; i < panum ; i++ )
	{
		SsPartAnime* panime = anime->partAnimes[i];
		partAnimeDic[panime->partName] = panime;
	}
	//パーツとパーツアニメを関連付ける
	size_t partNum = model->partList.size();

	if ( partState ) delete [] partState;
	partState = new SsPartState[partNum]();
	sortList.clear();
	partAnime.clear();

	for ( size_t i = 0 ; i < partNum ; i++ ) 
	{
		SsPart* p = model->partList[i];

		SsPartAndAnime _temp;
		_temp.first = p;
		_temp.second = partAnimeDic[p->name];
		partAnime.push_back( _temp );

		//親子関係の設定
		if ( p->parentIndex != -1 )
		{
			partState[i].parent = &partState[p->parentIndex];
		}else{
			partState[i].parent = 0;
		}

		//継承率の設定
		partState[i].inheritRates = p->inheritRates;
		partState[i].index = i;


		//インスタンスパーツの場合の初期設定
		if ( p->type == SsPartType::instance )
		{
			if (sspj)
			{
				//参照アニメーションを取得
				SsAnimePack* refpack = sspj->findAnimationPack( p->refAnimePack );
				SsAnimation* refanime = refpack->findAnimation( p->refAnime );

				SsCellMapList* __cellmap = new SsCellMapList();
				__cellmap->set( sspj , refpack );
				SsAnimeDecoder* animedecoder = new SsAnimeDecoder();
				animedecoder->setAnimation( &refpack->Model , refanime , __cellmap , sspj );
				partState[i].refAnime = animedecoder;
				//親子関係を付ける
				animedecoder->partState[0].parent = &partState[i];
			}
		}
				
		sortList.push_back( &partState[i] );

	}


	//アニメの最大フレーム数を取得
	curAnimeEndFrame = anime->settings.frameCount;
	curAnimeFPS = anime->settings.fps;

}



//頂点変形アニメデータの取得
void	SsAnimeDecoder::SsInterpolationValue( int time , const SsKeyframe* leftkey , const SsKeyframe* rightkey , SsVertexAnime& v )
{
	//☆Mapを使っての参照なので高速化必須 
	//todo ロード時に SsVertexAnimeを作成してしまうようにする
	SsVertexAnime	lv;
	SsVertexAnime	rv;

	if ( rightkey == 0 ) //右側が０出会った場合
	{
		GetSsVertexAnime( leftkey , v );
		return ;
	}

	GetSsVertexAnime(leftkey,lv);
	GetSsVertexAnime(rightkey,rv);

	int range = rightkey->time - leftkey->time;
	float now = (float)(time - leftkey->time) / range;

	SsCurve curve;
	curve = leftkey->curve;
	if (leftkey->ipType == SsInterpolationType::bezier)
	{
		// ベジェのみキーの開始・終了時間が必要
		curve.startKeyTime = leftkey->time;
		curve.endKeyTime = rightkey->time;
	}

	for ( int i = 0 ; i < 4 ; i++ )
	{
		v.offsets[i].x = SsInterpolate( leftkey->ipType , now , lv.offsets[i].x , rv.offsets[i].x , &curve );	
		v.offsets[i].y = SsInterpolate( leftkey->ipType , now , lv.offsets[i].y , rv.offsets[i].y , &curve );	
	}

}




void	SsAnimeDecoder::SsInterpolationValue( int time , const SsKeyframe* leftkey , const SsKeyframe* rightkey , SsColorAnime& v )
{
	//☆Mapを使っての参照なので高速化必須
	if ( rightkey == 0 )
	{
		GetSsColorValue( leftkey , v );
		return ;
	}
	
	SsColorAnime leftv;
	SsColorAnime rightv;

	GetSsColorValue( leftkey , leftv );
	GetSsColorValue( rightkey , rightv );


	SsCurve curve;
	curve = leftkey->curve;
	if (leftkey->ipType == SsInterpolationType::bezier)
	{
		// ベジェのみキーの開始・終了時間が必要
		curve.startKeyTime = leftkey->time;
		curve.endKeyTime = rightkey->time;
	}

	int range = rightkey->time - leftkey->time;
	float now = (float)(time - leftkey->time) / range;

	if ( leftv.target == SsColorBlendTarget::vertex )
	{
		for ( int i = 0 ; i < 4 ; i++ )
		{
			v.colors[i].rgba.a = SsInterpolate( leftkey->ipType , now , leftv.colors[i].rgba.a , rightv.colors[i].rgba.a  , &curve );	
			v.colors[i].rgba.r = SsInterpolate( leftkey->ipType , now , leftv.colors[i].rgba.r , rightv.colors[i].rgba.r  , &curve );	
			v.colors[i].rgba.g = SsInterpolate( leftkey->ipType , now , leftv.colors[i].rgba.g , rightv.colors[i].rgba.g  , &curve );	
			v.colors[i].rgba.b = SsInterpolate( leftkey->ipType , now , leftv.colors[i].rgba.b , rightv.colors[i].rgba.b  , &curve );	
		}
	}else{
		v.color.rgba.a = SsInterpolate( leftkey->ipType , now , leftv.color.rgba.a , rightv.color.rgba.a  , &curve );	
		v.color.rgba.r = SsInterpolate( leftkey->ipType , now , leftv.color.rgba.r , rightv.color.rgba.r  , &curve );	
		v.color.rgba.g = SsInterpolate( leftkey->ipType , now , leftv.color.rgba.g , rightv.color.rgba.g  , &curve );	
		v.color.rgba.b = SsInterpolate( leftkey->ipType , now , leftv.color.rgba.b , rightv.color.rgba.b  , &curve );	
	}

}


void	SsAnimeDecoder::SsInterpolationValue( int time , const SsKeyframe* leftkey , const SsKeyframe* rightkey , SsCellValue& v )
{
	SsRefCell cell;
	GetSsRefCell( leftkey , cell );

	SsCelMapLinker* l = this->curCellMapManager->getCellMapLink( cell.mapid );
	v.cell = l->findCell( cell.name );

	v.filterMode = l->cellMap->filterMode;
	v.wrapMode = l->cellMap->wrapMode;

	if ( l->tex )
	{
		v.texture = l->tex;
	}else v.texture = 0;

	calcUvs( &v );

}

//float , int , bool基本型はこれで値の補間を行う
template<typename mytype>
void	SsAnimeDecoder::SsInterpolationValue( int time , const SsKeyframe* leftkey , const SsKeyframe* rightkey , mytype& v )
{
	if ( rightkey == 0 )
	{
		v = leftkey->value.get<mytype>();
		return ;
	}
	
	float v1 = (float)leftkey->value.get<mytype>();
	float v2 = (float)rightkey->value.get<mytype>();

	int range = rightkey->time - leftkey->time;
	float now = (float)(time - leftkey->time) / range;

	if (leftkey->ipType == SsInterpolationType::bezier)
	{
		// ベジェのみキーの開始・終了時間が必要
		SsCurve curve;
		curve = leftkey->curve;
		curve.startKeyTime = leftkey->time;
		curve.endKeyTime = (float)rightkey->time;
		v = SsInterpolate( leftkey->ipType , now , v1 , v2 , &curve );
	}
	else{
		v = SsInterpolate( leftkey->ipType , now , v1 , v2 , &leftkey->curve );
	}

}



template<typename mytype> int	SsAnimeDecoder::SsGetKeyValue( int time , SsAttribute* attr , mytype&  value )
{
	int	useTime = 0;

	if ( attr->isEmpty() )
	{
		//デフォルト値を入れる まだ未実装
		return useTime;
	}

	const SsKeyframe* lkey = attr->findLeftKey( time );

	//無い場合は、最初のキーを採用する
	if ( lkey == 0 )
	{
		lkey =  attr->firstKey();
		SsInterpolationValue( time , lkey , 0 , value );

		useTime = lkey->time;
		return useTime;

	}else if ( lkey->time == time )
	{
		SsInterpolationValue( time , lkey , 0 , value );
		useTime = time;
		return useTime;
	}else{
		//補間計算をする
		const SsKeyframe* rkey = attr->findRightKey( time );
		if (rkey == NULL)
		{
			// 次のキーが無いので補間できない。よって始点キーの値をそのまま返す
			SsInterpolationValue( time , lkey , 0 , value );
			useTime = lkey->time;
			return useTime;
		}else
		if (lkey->ipType == SsInterpolationType::none)
		{
			// 補間なし指定なので始点キーの値…
			SsInterpolationValue( time , lkey , 0 , value );
			useTime = lkey->time;
			return useTime ;
		}else
		{
			SsInterpolationValue( time , lkey ,rkey , value );
			useTime = time;
		}
	}
		
	return useTime ;

}


/*

//中間点を求める
static void	CoordinateGetDiagonalIntersection( SsVector2& out , const SsVector2& LU , const SsVector2& RU , const SsVector2& LD , const SsVector2& RD )
{
	out = SsVector2(0.f,0.f);

	// <<< 係数を求める >>>
	float c1 = (LD.y - RU.y) * (LD.x - LU.x) - (LD.x - RU.x) * (LD.y - LU.y);
	float c2 = (RD.x - LU.x) * (LD.y - LU.y) - (RD.y - LU.y) * (LD.x - LU.x);
	float c3 = (RD.x - LU.x) * (LD.y - RU.y) - (RD.y - LU.y) * (LD.x - RU.x);


	if ( c3 <= 0 && c3 >=0) return;

	float ca = c1 / c3;
	float cb = c2 / c3;

	// <<< 交差判定 >>>
	if(((0.0f <= ca) && (1.0f >= ca)) && ((0.0f <= cb) && (1.0f >= cb)))
	{
		// 交差している
		out.x = LU.x + ca * (RD.x - LU.x);
		out.y = LU.y + ca * (RD.y - LU.y);
	}
}

static SsVector2 GetLocalScale( float matrix[16] )
{
	float sx = SsVector2::distance( SsVector2( matrix[0] , matrix[1] ) , SsVector2( 0 , 0 ) );
	float sy = SsVector2::distance( SsVector2( matrix[4 * 1 + 0] , matrix[4 * 1 + 1] ) , SsVector2( 0 , 0 ) );

	return SsVector2( sx , sy );
}

*/

///現在の時間からパーツのアトリビュートの補間値を計算する
void	SsAnimeDecoder::updateState( int nowTime , SsPart* part , SsPartAnime* anime , SsPartState* state )
{

	//ステートの初期値を設定
	state->init();
	state->inheritRates = part->inheritRates;
	if ( anime == 0 ){
		IdentityMatrix( state->matrix );
		return ;
	}

	// 親の継承設定を引用する設定の場合、ここで参照先を親のものに変えておく。
	if (part->inheritType == SsInheritType::parent)
	{
		if ( state->parent )
		{
			state->inheritRates = state->parent->inheritRates;
		}
	}

	bool	size_x_key_find = false;
	bool	size_y_key_find = false;

	state->is_vertex_transform = false;
	state->is_color_blend = false;
	state->alphaBlendType = part->alphaBlendType;


	bool hidekey_find = false;

	if ( !anime->attributes.empty() )
	{
		foreach( SsAttributeList , anime->attributes , e )
		{
			SsAttribute* attr = (*e);
			switch( attr->tag )
			{
				case SsAttributeKind::invalid:	///< 無効値。旧データからの変換時など
					break;
				case SsAttributeKind::cell:		///< 参照セル
					{
						SsGetKeyValue( nowTime , attr , state->cellValue );
						state->noCells = false;
					}
					break;
				case SsAttributeKind::posx:		///< 位置.X
					SsGetKeyValue( nowTime , attr , state->position.x );
					break;
				case SsAttributeKind::posy:		///< 位置.Y
					SsGetKeyValue( nowTime , attr , state->position.y );
					break;
				case SsAttributeKind::posz:		///< 位置.Z
					SsGetKeyValue( nowTime , attr , state->position.z );
					break;
				case SsAttributeKind::rotx:		///< 回転.X
					SsGetKeyValue( nowTime , attr , state->rotation.x );
					break;
				case SsAttributeKind::roty:		///< 回転.Y
					SsGetKeyValue( nowTime , attr , state->rotation.y );
					break;
				case SsAttributeKind::rotz:		///< 回転.Z
					SsGetKeyValue( nowTime , attr , state->rotation.z );
					break;
				case SsAttributeKind::sclx:		///< スケール.X
					SsGetKeyValue( nowTime , attr , state->scale.x );
					break;
				case SsAttributeKind::scly:		///< スケール.Y
					SsGetKeyValue( nowTime , attr , state->scale.y );
					break;
				case SsAttributeKind::alpha:	///< 不透明度
					SsGetKeyValue( nowTime , attr , state->alpha );
					break;
				case SsAttributeKind::prio:		///< 優先度
					SsGetKeyValue( nowTime , attr , state->prio );
					break;
				case SsAttributeKind::fliph:	///< 左右反転(セルの原点を軸にする)
					SsGetKeyValue( nowTime , attr , state->hFlip );
					break;
				case SsAttributeKind::flipv:	///< 上下反転(セルの原点を軸にする)
					SsGetKeyValue( nowTime , attr , state->vFlip );
					break;
				case SsAttributeKind::hide:		///< 非表示
					{
						int useTime = SsGetKeyValue( nowTime , attr , state->hide );
						// 非表示キーがないか、先頭の非表示キーより手前の場合は常に非表示にする。
						if ( useTime > nowTime )
						{
							state->hide = true;
						}
						hidekey_find = true;
					}
					break;
				case SsAttributeKind::color:	///< カラーブレンド
					SsGetKeyValue( nowTime , attr , state->colorValue );
					state->is_color_blend = true;
					break;
				case SsAttributeKind::vertex:	///< 頂点変形
					SsGetKeyValue( nowTime , attr , state->vertexValue );
					state->is_vertex_transform = true;
					break;
				case SsAttributeKind::pivotx:	///< 原点オフセット.X
					SsGetKeyValue( nowTime , attr , state->pivotOffset.x );
					break;
				case SsAttributeKind::pivoty:	///< 原点オフセット.Y
					SsGetKeyValue( nowTime , attr , state->pivotOffset.y );
					break;
				case SsAttributeKind::anchorx:	///< アンカーポイント.X
					SsGetKeyValue( nowTime , attr , state->anchor.x );
					break;
				case SsAttributeKind::anchory:	///< アンカーポイント.Y
					SsGetKeyValue( nowTime , attr , state->anchor.y );
					break;
				case SsAttributeKind::sizex:	///< 表示サイズ.X
					SsGetKeyValue( nowTime , attr , state->size.x );
					size_x_key_find = true;
					break;
				case SsAttributeKind::sizey:	///< 表示サイズ.Y
					SsGetKeyValue( nowTime , attr , state->size.y );
					size_y_key_find = true;
					break;
				case SsAttributeKind::imgfliph:	///< イメージ左右反転(常にイメージの中央を原点とする)
					SsGetKeyValue( nowTime , attr , state->imageFlipH );
					break;
				case SsAttributeKind::imgflipv:	///< イメージ上下反転(常にイメージの中央を原点とする)
					SsGetKeyValue( nowTime , attr , state->imageFlipV );
					break;
				case SsAttributeKind::uvtx:		///< UVアニメ.移動.X
					SsGetKeyValue( nowTime , attr , state->uvTranslate.x );
					break;
				case SsAttributeKind::uvty:		///< UVアニメ.移動.Y
					SsGetKeyValue( nowTime , attr , state->uvTranslate.y );
					break;
				case SsAttributeKind::uvrz:		///< UVアニメ.回転
					SsGetKeyValue( nowTime , attr , state->uvRotation );
					break;
				case SsAttributeKind::uvsx:		///< UVアニメ.スケール.X
					SsGetKeyValue( nowTime , attr , state->uvScale.x );
					break;
				case SsAttributeKind::uvsy:		///< UVアニメ.スケール.Y
					SsGetKeyValue( nowTime , attr , state->uvScale.y );
					break;
				case SsAttributeKind::boundr:	///< 当たり判定用の半径
					break;
				case SsAttributeKind::user:		///< Ver.4 互換ユーザーデータ
					break;
				case SsAttributeKind::instance:	///インスタンスパラメータ
					break;
			}
		}
	}



	// カラー値だけアニメが無いと設定されないので初期値を入れておく。
	// alpha はupdateで初期化されるのでOK
	// 当たり判定パーツ用のカラー。赤の半透明にする
	static const float sColorsForBoundsParts[] = {0.5f, 0.f, 0.f, 1.f};
	for (int i = 0; i < (4*4) ; ++i)
	{
		if (state->noCells)
			state->colors[i] = sColorsForBoundsParts[i & 3];
		else
			state->colors[i] = 1.f;
	}

	// 継承
	if (state->parent)
	{
		// α
		if (state->inherits_(SsAttributeKind::alpha))
			state->alpha *= state->parent->alpha;
		// フリップの継承。継承ONだと親に対しての反転になる…ヤヤコシス
		if (state->inherits_(SsAttributeKind::fliph))
		{
			state->hFlip = state->parent->hFlip ^ state->hFlip;
		}

		if (state->inherits_(SsAttributeKind::flipv))
			state->vFlip = state->parent->vFlip ^ state->vFlip;
		// 非表示は継承ONだと親のをただ引き継ぐ
		if (state->inherits_(SsAttributeKind::hide))
			state->hide = state->parent->hide;
	}

	// 非表示キーがないか、先頭の非表示キーより手前の場合は常に非表示にする。(継承関係なし)
	if (!hidekey_find)
	{
		state->hide = true;
	}

	// 頂点の設定
	if ( part->type == SsPartType::normal )
	{
		SsCell * cell = state->cellValue.cell;
		if (cell && anime)
		{
			//サイズアトリビュートが指定されていない場合、セルのサイズを設定する
			if ( !size_x_key_find ) state->size.x = cell->size.x;
			if ( !size_y_key_find ) state->size.y = cell->size.y;
		}
		updateVertices(part , anime , state);
	}



}

void	SsAnimeDecoder::updateMatrix(SsPart* part , SsPartAnime* anime , SsPartState* state)
{

	IdentityMatrix( state->matrix );

	if (state->parent)
	{
		memcpy( state->matrix , state->parent->matrix , sizeof( float ) * 16 );
	}

	// アンカー
	if ( state->parent )
	{
		const SsVector2& parentSize = state->parent->size;
		state->position.x = state->position.x + state->parent->size.x * state->anchor.x;
		state->position.y = state->position.y + state->parent->size.y * state->anchor.y;
	}

	// DirectXは座標系が逆なので座標系を変更する
	TranslationMatrixM( state->matrix , state->position.x, -state->position.y, state->position.z );// Y座標移動を変更
	RotationXYZMatrixM( state->matrix , (float)DegreeToRadian(state->rotation.x) , (float)DegreeToRadian(state->rotation.y) , (float)DegreeToRadian( -state->rotation.z) );// Z回転が反対なので変更
	ScaleMatrixM( state->matrix, state->scale.x, state->scale.y, 1.0f );

#if 0
	// 当たり判定用半径をスケーリングする。テスト
	if (boundingRadius != 0)
	{
		if ( part->boundsType == SsBoundsType::circle_smin )
		{
			SsVector2 local_scale = GetLocalScale(matrix);
			boundingRadius = boundingRadiusOrg * (local_scale.x <= local_scale.y ? local_scale.x : local_scale.y);
		}

		if ( part->boundsType == SsBoundsType::circle_smax )
		{
			SsVector2 local_scale = GetLocalScale(matrix);
			boundingRadius = boundingRadiusOrg * ( local_scale.x >= local_scale.y ? local_scale.x : local_scale.y);
		}
	}
#endif

}


void	SsAnimeDecoder::updateVertices(SsPart* part , SsPartAnime* anime , SsPartState* state)
{

	SsCell * cell = state->cellValue.cell;

	SsVector2 pivot;
	if (cell)
	{
		// セルに設定された原点オフセットを適用する
		// ※セルの原点は中央が0,0で＋が右上方向になっている
		float cpx = cell->pivot.x + 0.5f;
		if (state->hFlip) cpx = 1 - cpx;	// 水平フリップによって原点を入れ替える
		pivot.x = cpx * state->size.x;
		// DirectXでは座標系が逆なので反対にする
		float cpy = cell->pivot.y + 0.5f;
		if (state->vFlip) cpy = 1 - cpy;	// 垂直フリップによって原点を入れ替える
		pivot.y = cpy * state->size.y;
	}
	else
	{
		// セルが無いパーツでも原点が中央に来るようにする。
		pivot.x = 0.5f * state->size.x;
		pivot.y = 0.5f * state->size.y;
	}

	// 次に原点オフセットアニメの値を足す
	pivot.x += state->pivotOffset.x * state->size.x;
	pivot.y += state->pivotOffset.y * state->size.y;// DirectXでは座標系が逆なので反対にする

	float sx = -pivot.x;
	float ex = sx + state->size.x;
	float sy = +pivot.y;
	float ey = sy - state->size.y;

	// Z順
	/*
		これは実は上下ひっくり返って裏面になっているためUV値も上下反転させている。
		左上が最初に来る並びの方が頂点カラー・頂点変形のデータと同じで判りやすいのでこれでいく。
	*/
	float vtxPosX[4] = {sx, ex, sx, ex};
	float vtxPosY[4] = {sy, sy, ey, ey};

	SsPoint2 * vtxOfs = state->vertexValue.offsets;

	//きれいな頂点変形に対応
#if USE_TRIANGLE_FIN

	if ( state->is_color_blend || state->is_vertex_transform )
	{

		SsVector2	vertexCoordinateLU = SsVector2( sx + (float)vtxOfs[0].x , sy + (float)vtxOfs[0].y );// : 左上頂点座標（ピクセル座標系）
		SsVector2	vertexCoordinateRU = SsVector2( ex + (float)vtxOfs[1].x , sy + (float)vtxOfs[1].y );// : 右上頂点座標（ピクセル座標系）
		SsVector2	vertexCoordinateLD = SsVector2( sx + (float)vtxOfs[2].x , ey + (float)vtxOfs[2].y );// : 左下頂点座標（ピクセル座標系）
		SsVector2	vertexCoordinateRD = SsVector2( ex + (float)vtxOfs[3].x , ey + (float)vtxOfs[3].y );// : 右下頂点座標（ピクセル座標系）

		SsVector2 CoordinateLURU = (vertexCoordinateLU + vertexCoordinateRU) * 0.5f;
		SsVector2 CoordinateLULD = (vertexCoordinateLU + vertexCoordinateLD) * 0.5f;
		SsVector2 CoordinateLDRD = (vertexCoordinateLD + vertexCoordinateRD) * 0.5f;
		SsVector2 CoordinateRURD = (vertexCoordinateRU + vertexCoordinateRD) * 0.5f;

		SsVector2 center;
		CoordinateGetDiagonalIntersection( center , CoordinateLURU, CoordinateRURD, CoordinateLULD, CoordinateLDRD );

		SsVector2*	coodinatetable[] = { &vertexCoordinateLU , &vertexCoordinateRU , &vertexCoordinateLD , &vertexCoordinateRD , &center };


		for (int i = 0; i < 5; ++i)
		{
			state->vertices[ i * 3 ] = coodinatetable[i]->x;
			state->vertices[ i * 3 + 1 ] = coodinatetable[i]->y;
			state->vertices[ i * 3 + 2]	= 0;
		}
	}else{
		for (int i = 0; i < 4; ++i)
		{
			state->vertices[i * 3]		= vtxPosX[i];
			state->vertices[i * 3 + 1]	= vtxPosY[i];
			state->vertices[i * 3 + 2]	= 0;

			++vtxOfs;
		}

	}

#else
	// DirectXの座標系に合わせる
	state->vertices[ 0 ] = vtxPosX[ 0 ] + (float)vtxOfs[ 2 ].x;
	state->vertices[ 1 ] = vtxPosY[ 0 ] + (float)vtxOfs[ 2 ].y;
	state->vertices[ 2 ] = 0;
	state->vertices[ 3 ] = vtxPosX[ 1 ] + (float)vtxOfs[ 3 ].x;
	state->vertices[ 4 ] = vtxPosY[ 1 ] + (float)vtxOfs[ 3 ].y;
	state->vertices[ 5 ] = 0;
	state->vertices[ 6 ] = vtxPosX[ 2 ] + (float)vtxOfs[ 0 ].x;
	state->vertices[ 7 ] = vtxPosY[ 2 ] + (float)vtxOfs[ 0 ].y;
	state->vertices[ 8 ] = 0;
	state->vertices[ 9 ] = vtxPosX[ 3 ] + (float)vtxOfs[ 1 ].x;
	state->vertices[ 10 ] = vtxPosY[ 3 ] + (float)vtxOfs[ 1 ].y;
	state->vertices[ 11 ] = 0;
#endif



}



void	SsAnimeDecoder::updateInstance( int nowTime , SsPart* part , SsPartAnime* partanime , SsPartState* state )
{
	if ( state->refAnime == 0 ) return ;
	//state->refAnime->setPlayFrame( nowTime );
	//state->refAnime->update();

    //ラベル未実装だが正規の方法で計算しとく

//	SsAnimation* anime = state->refAnime;

	const SsInstanceAttr& instanceValue = state->instanceValue;

#if 1	//なんとかラベルをゲットできる仕組みを・・・
    //プレイヤー等では再生開始時にいったん計算してしまって値にしてしまった方がいい。
    //エディター側のロジックなのでそのまま検索する
    //インスタンスアニメ内のスタート位置
    int	startframe = CalcAnimeLabel2Frame( instanceValue.startLabel , instanceValue.startOffset);
    int	endframe = CalcAnimeLabel2Frame( instanceValue.endLabel , instanceValue.endOffset);
#else
    int	startframe = 0;
    int	endframe = curAnimeEndFrame;
#endif

    //タイムライン上の時間 （絶対時間）
	int time = nowTime;

	//独立動作の場合
	if ( instanceValue.independent )
	{
		//float delta = animeState->frame - parentBackTime;
		float delta = this->frameDelta;

		state->instanceValue.liveFrame+= ( delta * instanceValue.speed );
		//parentBackTime = animeState->frame;
		time = (int)instanceValue.liveFrame;

	}

    //このインスタンスが配置されたキーフレーム（絶対時間）
    int	selfTopKeyframe = instanceValue.curKeyframe;


    int	reftime = (time*instanceValue.speed) - selfTopKeyframe; //開始から現在の経過時間
    if ( reftime < 0 ) return ; //そもそも生存時間に存在していない

    int inst_scale = (endframe - startframe) + 1; //インスタンスの尺


	//尺が０もしくはマイナス（あり得ない
	if ( inst_scale <= 0 ) return ;

	int	nowloop =  (reftime / inst_scale);	//現在までのループ数

    int checkloopnum = instanceValue.loopNum;

	//pingpongの場合では２倍にする
    if ( instanceValue.pingpong ) checkloopnum = checkloopnum * 2;

	//無限ループで無い時にループ数をチェック
    if ( !instanceValue.infinity )   //無限フラグが有効な場合はチェックせず
	{
        if ( nowloop >= checkloopnum )
		{
			reftime = inst_scale-1;
			nowloop = checkloopnum-1;
		}
	}

	int temp_frame = reftime % inst_scale;  //ループを加味しないインスタンスアニメ内のフレーム

    //参照位置を決める
    //現在の再生フレームの計算
    int _time = 0;
	bool	reverse = instanceValue.reverse;
	if ( instanceValue.pingpong && (nowloop % 2 == 1) )
	{
       reverse=!reverse;//反転
	}

	if (reverse)
	{
		//リバースの時
		_time = endframe - temp_frame;
	}else{
		//通常時
		_time = temp_frame + startframe;
    }

/*	//エディタではないから仮のサイズ値は要らない？
	//再生
    state->size.x = part->anime->settings.canvasSize.x;
    state->size.y = part->anime->settings.canvasSize.y;
*/

	state->refAnime->setPlayFrame( _time );
	state->refAnime->update();

  
	//頂点の作成
    //update_vertices();

}

int		SsAnimeDecoder::findAnimetionLabel(const SsString& str)
{
	for ( std::vector<SsLabel*>::iterator itr = curAnimation->labels.begin() ; 
		itr != curAnimation->labels.end() ; itr ++ )
	{
		if ( str == (*itr)->name )
		{
			return (*itr)->time;
		}
	}

	return 0;
}

int		SsAnimeDecoder::CalcAnimeLabel2Frame(const SsString& str, int offset )
{

	int maxframe = curAnimeEndFrame;
    int ret2 = offset;

    if (  str == "_start" )
	{
    	return offset;
	}else if ( str == "_end" )
	{
        return maxframe + offset;
	}else if ( str == "none" )
	{
        return offset;
	}else{
		int ret = findAnimetionLabel(str);

        if ( ret != -1 )
        {
			int ret2 = ret + offset;
            if ( ret2 < 0 ) ret2 = 0;
            if ( ret2 > maxframe ) ret2 = maxframe;

        	return ret2;
		}
		//警告など出すべき？
	}

    if ( ret2 < 0 ) ret2 = 0;
    if ( ret2 > maxframe ) ret2 = maxframe;

	return ret2;



}


static SsPartStateLess _ssPartStateLess;

///SS5の場合  SsPartのarrayIndexは、親子順　（子は親より先にいない）と
///なっているためそのまま木構造を作らずUpdateを行う
void	SsAnimeDecoder::update()
{
	int	time = (int)nowPlatTime;

	int cnt = 0;
	foreach( std::vector<SsPartAndAnime> , partAnime , e )
	{
		SsPart* part = e->first;
		SsPartAnime* anime = e->second;
		updateState( time , part , anime , &partState[cnt] );
		updateMatrix( part , anime , &partState[cnt]);

		if ( part->type == SsPartType::instance )
		{
			updateInstance( time , part , anime , &partState[cnt] );
			updateVertices( part , anime , &partState[cnt] );
		}

		cnt++;
	}

	sortList.sort(_ssPartStateLess);

}



//描画
void	SsAnimeDecoder::draw()
{

	SsCurrentRenderer::getRender()->renderSetup();

	foreach( std::list<SsPartState*> , sortList , e )
	{
		SsPartState* state = (*e);

		if ( state->refAnime )
		{
			state->refAnime->draw();
		}else{
			SsCurrentRenderer::getRender()->renderPart(state);
		}
	}
}

#pragma warning( pop )
