#include <stdio.h>
#include <cstdlib>

#ifndef _WIN32
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#include <GL/GL.h>
#endif

//#include <GLFW/glfw3.h>


#include "ssplayer_player.h"
#include "ssplayer_matrix.h"
#include "ssplayer_shader.h"


#define __PI__	(3.14159265358979323846f)
#define RadianToDegree(Radian) ((double)Radian * (180.0f / __PI__))
#define DegreeToRadian(Degree) ((double)Degree * (__PI__ / 180.0f))

//stdでののforeach宣言　
#define foreach(T, c, i) for(T::iterator i = c.begin(); i!=c.end(); ++i)
#define USE_TRIANGLE_FIN (0)
#define PROGRAMABLE_SHADER_ON (1)

static const char* glshader_sprite_vs = 
#include "shader/sprite.vs";

static const char* glshader_sprite_fs = 
#include "shader/sprite.fs";

static const char* glshader_sprite_fs_pot = 
#include "shader/sprite_pot.fs";





enum{
	PG_SHADER_NPOT,
	PG_SHADER_POT,
};



void	SsPlayer::shaderInit()
{
	
	SSOpenGLShaderMan::Create();
	SSOpenGLVertexShader*	vs = new SSOpenGLVertexShader( "basec_vs" , glshader_sprite_vs );
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

}



void	SsCellMapManager::clear()
{
	CellMapDic.clear();
}


void	SsCellMapManager::setCellMapPath(  const SsString& filepath )
{
	CellMapPath = filepath;
}

void	SsCellMapManager::add(SsProject* proj)
{
	for ( size_t i = 0 ; i < proj->cellmapList.size() ; i++ )
	{
		add( proj->cellmapList[i] );
	}
}

void	SsCellMapManager::set(SsProject* proj)
{
	CellMapDic.clear();
	setCellMapPath( proj->getImageBasepath() );
	add( proj );
}

void	SsCellMapManager::add(SsCellMap* cellmap)
{
	SsCelMapLinker* linker = new SsCelMapLinker(cellmap , this->CellMapPath );
	CellMapDic[ cellmap->name+".ssce" ] = linker ;
}

SsCelMapLinker*	SsCellMapManager::getCellMapLink( const SsString& name )
{
	SsCelMapLinker* l = CellMapDic[name];
	if ( l != 0 ) return l;

	return 0;
}


static void calcUvs( SsCellValue* cellv )
{
	SsCellMap* map = cellv->cellmapl->cellMap;
	SsCell* cell = cellv->cell;

	if ( cell == 0 || map == 0)
	{
		cellv->uvs[0].x = cellv->uvs[0].y = 0;
		cellv->uvs[1].x = cellv->uvs[1].y = 0;
		cellv->uvs[2].x = cellv->uvs[2].y = 0;
		cellv->uvs[3].x = cellv->uvs[3].y = 0;
		return;
	}

	SsVector2 wh = map->pixelSize;
	// 右上に向かって＋になる
	float left = cell->pos.x / wh.x;
	float right = (cell->pos.x + cell->size.x) / wh.x;


	// LB->RB->LT->RT 順
	// 頂点をZ順にしている都合上UV値は上下逆転させている
	float top = cell->pos.y / wh.y;
	float bottom = ( cell->pos.y + cell->size.y) / wh.y;

	if (cell->rotated)
	{
		// 反時計回りに９０度回転されているため起こして描画されるようにしてやる。
		// 13
		// 02
		cellv->uvs[0].x = cellv->uvs[1].x = left;
		cellv->uvs[2].x = cellv->uvs[3].x = right;
		cellv->uvs[1].y = cellv->uvs[3].y = top;
		cellv->uvs[0].y = cellv->uvs[2].y = bottom;
	}
	else
	{
		// そのまま。頂点の順番は下記の通り
		// 01
		// 23
		cellv->uvs[0].x = cellv->uvs[2].x = left;
		cellv->uvs[1].x = cellv->uvs[3].x = right;
		cellv->uvs[0].y = cellv->uvs[1].y = top;
		cellv->uvs[2].y = cellv->uvs[3].y = bottom;
	}
}


SsPlayer::SsPlayer() : 
	nowPlatTime(0) ,
	project(0) ,
	curAnime(0) ,
	curModel(0) ,
	curAnimePack(0) ,
	curCellMapManager(0),
	partState(0)
	{
		//AnimationChangeMsg.
		AnimationChangeMsg.change = false;
		AnimationChangeMsg.animePack = 0;
		AnimationChangeMsg.anime = 0;
	}


void SsPlayer::setProject( SsProject* _proj , int packIndex , int animeIndex)
{
	SsProject* proj = _proj;
	if ( _proj == 0 ) proj = project;
	project = proj;


	//セルマップの関連付け
	//最終的には分離
	if ( curCellMapManager ) delete curCellMapManager;
	curCellMapManager = new SsCellMapManager();
	curCellMapManager->set( proj );

	//モデルデータの作成
	SsAnimePack* animePack = proj->animeList[packIndex];
	curAnimePack = animePack;
	curAnime = animePack->animeList[animeIndex];
	curModel = &animePack->Model;
    
    //UnicodeString str;
	//パーツアニメの名称を保持
	PartAnimeDic.clear();
	size_t anum = curAnime->partAnimes.size();
    
    
	for ( size_t i = 0 ; i < anum ; i++ )
	{
		SsPartAnime* panime = curAnime->partAnimes[i];
		PartAnimeDic[panime->partName] = panime;
	}

	//パーツとパーツアニメを関連付ける
	size_t partNum = curModel->partList.size();

	if ( partState ) delete [] partState;
	partState = new SsPartState[partNum]();
	sortList.clear();
	part_anime.clear();

	for ( size_t i = 0 ; i < partNum ; i++ ) 
	{
		SsPart* p = curModel->partList[i];

		SsPartAndAnime _temp;
		_temp.first = p;
		_temp.second = PartAnimeDic[p->name];
		part_anime.push_back( _temp );


		//親子関係
		if ( p->parentIndex != -1 )
		{
			partState[i].parent = &partState[p->parentIndex];
		}else{
			partState[i].parent = 0;
		}

		partState[i].inheritRates = p->inheritRates;
		partState[i].index = i;

		sortList.push_back( &partState[i] );
	}


	//アニメの最大フレーム数を取得
	curAnimeEndFrame = curAnime->settings.frameCount;

}

void	SsPlayer::changeAnimation(int packIndex, int animeIndex)
{
	AnimationChangeMsg.change = true;
	AnimationChangeMsg.animePack = packIndex;
	AnimationChangeMsg.anime = animeIndex;
}


void	SsInterpolationValue( int time , const SsKeyframe* leftkey , const SsKeyframe* rightkey , SsVertexAnime& v )
{
	//☆Mapを使っての参照なので高速化必須

	SsVertexAnime	lv;
	SsVertexAnime	rv;

	const SsString& sLLT = leftkey->value["LT"].get<SsString>();
	const SsString& sLRT = leftkey->value["RT"].get<SsString>();
	const SsString& sLLB = leftkey->value["LB"].get<SsString>();
	const SsString& sLRB = leftkey->value["RB"].get<SsString>();
	
	if ( rightkey == 0 )
	{
		StringToPoint2( sLLT , v.offsets[0] );
		StringToPoint2( sLRT , v.offsets[1] );
		StringToPoint2( sLLB , v.offsets[2] );
		StringToPoint2( sLRB , v.offsets[3] );
		return ;
	}
	StringToPoint2( sLLT , lv.offsets[0] );
	StringToPoint2( sLRT , lv.offsets[1] );
	StringToPoint2( sLLB , lv.offsets[2] );
	StringToPoint2( sLRB , lv.offsets[3] );


	const SsString& sRLT = rightkey->value["LT"].get<SsString>();
	const SsString& sRRT = rightkey->value["RT"].get<SsString>();
	const SsString& sRLB = rightkey->value["LB"].get<SsString>();
	const SsString& sRRB = rightkey->value["RB"].get<SsString>();
	
	StringToPoint2( sRLT , rv.offsets[0] );
	StringToPoint2( sRRT , rv.offsets[1] );
	StringToPoint2( sRLB , rv.offsets[2] );
	StringToPoint2( sRRB , rv.offsets[3] );
	
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



static void	GetColorValue( const SsKeyframe* key , SsColorAnime& v )
{
	SsColorBlendTarget::_enum target;
	__StringToEnum_( key->value["target"].get<SsString>() , target );
		SsBlendType::_enum blendtype;
	__StringToEnum_( key->value["blendType"].get<SsString>() , blendtype);

	v.blendType = blendtype;
	v.target = target;

	if ( target == SsColorBlendTarget::vertex )
	{
		SsHash lt = key->value["LT"].get<SsHash>();
		SsHash rt = key->value["RT"].get<SsHash>();
		SsHash lb = key->value["LB"].get<SsHash>();
		SsHash rb = key->value["RB"].get<SsHash>();

		ConvertStringToSsColor( lt["rgba"].get<SsString>() , v.colors[0].rgba);
		v.colors[0].rate = lt["rate"].get<float>();

		ConvertStringToSsColor( rt["rgba"].get<SsString>() , v.colors[2].rgba);
		v.colors[1].rate = rt["rate"].get<float>();

		ConvertStringToSsColor( lb["rgba"].get<SsString>() , v.colors[2].rgba);
		v.colors[2].rate = lb["rate"].get<float>();

		ConvertStringToSsColor( rb["rgba"].get<SsString>() , v.colors[3].rgba);
		v.colors[3].rate = rb["rate"].get<float>();

	}else{
		SsHash color = key->value["color"].get<SsHash>();
		ConvertStringToSsColor( color["rgba"].get<SsString>() , v.color.rgba);
		v.color.rate = color["rate"].get<float>();
	}

}

void	SsInterpolationValue( int time , const SsKeyframe* leftkey , const SsKeyframe* rightkey , SsColorAnime& v )
{
	//☆Mapを使っての参照なので高速化必須
	if ( rightkey == 0 )
	{
		GetColorValue( leftkey , v );
		return ;
	}

	SsColorAnime leftv;
	SsColorAnime rightv;

	GetColorValue( leftkey , leftv );
	GetColorValue( rightkey , rightv );


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


void	SsInterpolationValue( int time , const SsKeyframe* leftkey , const SsKeyframe* rightkey , SsCellValue& v )
{
	int id = leftkey->value["mapId"].get<int>();

	//leftkey->value["name"].get<SsString>();
	SsString name = leftkey->value["name"].get<SsString>();

	//☆Mapを使っての参照なので高速化必須
	SsString& str = v.player->curAnimePack->cellmapNames[id];
	SsCelMapLinker* l = v.player->curCellMapManager->getCellMapLink( str );
	v.cell = l->findCell( name );
	v.cellmapl = l;
	if ( l->tex )
	{
		v.texture = l->tex->tex;
	}else v.texture = 0;

	calcUvs( &v );
	
}

//float , int , bool基本型はこれで値の補間を行う
template<typename mytype>
void	SsInterpolationValue( int time , const SsKeyframe* leftkey , const SsKeyframe* rightkey , mytype& v )
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


template<class mytype>
int	SsGetKeyValue( int time , SsAttribute* attr , mytype&  value )
{
	int	useTime = 0;

	if ( attr->isEmpty() )
	{
		//デフォルト値を入れる まだ未実装
		return useTime;
	}

    if ( time == 1 )
    {
        int a = 0;
        a ++;
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


//中間点を求める
static void	CoordinateGetDiagonalIntersection( SsVector2& out , const SsVector2& LU , const SsVector2& RU , const SsVector2& LD , const SsVector2& RD )
{
	out = SsVector2(0.f,0.f);

	/* <<< 係数を求める >>> */
	float c1 = (LD.y - RU.y) * (LD.x - LU.x) - (LD.x - RU.x) * (LD.y - LU.y);
	float c2 = (RD.x - LU.x) * (LD.y - LU.y) - (RD.y - LU.y) * (LD.x - LU.x);
	float c3 = (RD.x - LU.x) * (LD.y - RU.y) - (RD.y - LU.y) * (LD.x - RU.x);


	if ( c3 <= 0 && c3 >=0) return;

	float ca = c1 / c3;
	float cb = c2 / c3;

	/* <<< 交差判定 >>> */
	if(((0.0f <= ca) && (1.0f >= ca)) && ((0.0f <= cb) && (1.0f >= cb)))
	{	/* 交差している */
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


///現在の時間からパーツのアトリビュートの補間値を計算する
void	SsPlayer::updateState( int nowTime , SsPart* part , SsPartAnime* anime , SsPartState* state )
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
						state->cellValue.player = this;
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

/*本体機能がリリース前なので一度コメント
	// 非表示キーがないか、先頭の非表示キーより手前の場合は常に非表示にする。(継承関係なし)
	if (!hidekey_find)
	{
		state->hide = true;
	}
*/
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

void	SsPlayer::updateMatrix(SsPart* part , SsPartAnime* anime , SsPartState* state)
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

	TranslationMatrixM( state->matrix , state->position.x, state->position.y, state->position.z );//
	RotationXYZMatrixM( state->matrix , DegreeToRadian(state->rotation.x) , DegreeToRadian(state->rotation.y) , DegreeToRadian( state->rotation.z) );
	ScaleMatrixM(  state->matrix , state->scale.x, state->scale.y, 1.0f );

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


void	SsPlayer::updateVertices(SsPart* part , SsPartAnime* anime , SsPartState* state)
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
		// 上が＋で入っているのでここで反転する。
		float cpy = -cell->pivot.y + 0.5f;
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
	pivot.y += -state->pivotOffset.y * state->size.y;

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
	//4点変形の場合
	// 頂点変形のデータはＺ字順に格納されている。
	//SsPoint2 * vtxOfs = vertexValue.offsets;
	for (int i = 0; i < 4; ++i)
	{
		state->vertices[i * 3]		= vtxPosX[i] + (float)vtxOfs->x;
		state->vertices[i * 3 + 1]	= vtxPosY[i] + (float)vtxOfs->y;
		state->vertices[i * 3 + 2]	= 0;

		++vtxOfs;
	}
#endif



}


///SS5の場合  SsPartのarrayIndexは、親子順　（子は親より先にいない）と
///なっているためそのまま木構造を作らずUpdateを行う
void	SsPlayer::update()
{
	if ( AnimationChangeMsg.change )
	{
		this->setProject( 0 , AnimationChangeMsg.animePack , AnimationChangeMsg.anime );
		AnimationChangeMsg.change = false;
	}

	int	time = (int)nowPlatTime;

	int cnt = 0;
	foreach( std::vector<SsPartAndAnime> , part_anime , e )
	{
		SsPart* part = e->first;
		SsPartAnime* anime = e->second;
		updateState( time , part , anime , &partState[cnt] );
		updateMatrix( part , anime , &partState[cnt]);
		cnt++;
	}

}

class SsPartStateLess
{
public:
	bool operator()(const SsPartState* lhs, const SsPartState* rhs) const
	{
		if (lhs->prio == rhs->prio)
			return lhs->index < rhs->index;
		return lhs->prio < rhs->prio;
	}
};
static SsPartStateLess _ssPartStateLess;

void	SsPlayer::draw()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	sortList.sort(_ssPartStateLess);


	foreach( std::list<SsPartState*> , sortList , e )
	{
		SsPartState* state = (*e);
		draw_part( state );
	}

}



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

void	SsPlayer::draw_part( SsPartState* state )
{
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
	int		cellimage = state->cellValue.texture;
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
		if (SsGL_isPow2(map->pixelSize.x) &&
			SsGL_isPow2(map->pixelSize.y))
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
		glBindTexture(gl_target, cellimage);

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

}
