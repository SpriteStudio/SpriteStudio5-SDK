#include "SpriteStudio5PrivatePCH.h"
#include "SsPlayerAnimedecode.h"

#include <stdio.h>
#include <cstdlib>

#include "SsProject.h"
#include "SsCellMap.h"
#include "SsAnimePack.h"
#include "SsAttribute.h"
#include "SsInterpolation.h"
#include "SsPlayerMatrix.h"
#include "SsPlayerPartState.h"
#include "SsPlayerCellmap.h"



#define __PI__	(3.14159265358979323846f)
#define RadianToDegree(Radian) ((double)Radian * (180.0f / __PI__))
#define DegreeToRadian(Degree) ((double)Degree * (__PI__ / 180.0f))

#define USE_TRIANGLE_FIN (0)

FSsAnimeDecoder::FSsAnimeDecoder() : 
	CurAnimeFPS(0),
	CurAnimeEndFrame(0),
	NowPlatTime(0) ,
	FrameDelta(0) ,
	CurCellMapManager(0),
	PartState(0)
{
}

FSsAnimeDecoder::~FSsAnimeDecoder()
{
	if ( PartState )
		delete [] PartState;
}



void FSsAnimeDecoder::SetAnimation(FSsModel* model, FSsAnimation* anime, FSsCellMapList* cellmap, USsProject* sspj)
{
	//セルマップリストを取得
	CurCellMapManager = cellmap;
	CurAnimation = anime;

	//partStateをパーツ分作成する
	PartAnimeDic.Empty();

	//パーツの数
	size_t panum = anime->PartAnimes.Num();
	for ( size_t i = 0 ; i < panum ; i++ )
	{
		FSsPartAnime* panime = &anime->PartAnimes[i];
		if(!PartAnimeDic.Contains(panime->PartName))
		{
			PartAnimeDic.Add(panime->PartName);
		}
		PartAnimeDic[panime->PartName] = panime;
	}
	//パーツとパーツアニメを関連付ける
	size_t partNum = model->PartList.Num();

	if ( PartState ) delete [] PartState;
	PartState = new FSsPartState[partNum]();
	SortList.Empty();
	PartAnime.Empty();

	for ( size_t i = 0 ; i < partNum ; i++ ) 
	{
		FSsPart* p = &model->PartList[i];

		if(!PartAnimeDic.Contains(p->PartName))
		{
			PartAnimeDic.Add(p->PartName);	// std::map互換のため、存在しないキーに対して要素NULLで追加する.
		}
		FSsPartAndAnime _temp;
		_temp.Key = p;
		_temp.Value = PartAnimeDic[p->PartName];
		PartAnime.Add( _temp );

		//親子関係の設定
		if ( p->ParentIndex != -1 )
		{
			PartState[i].Parent = &PartState[p->ParentIndex];
		}else{
			PartState[i].Parent = 0;
		}

		//継承率の設定
		PartState[i].InheritRates = p->InheritRates;
		PartState[i].Index = i;


		//インスタンスパーツの場合の初期設定
		if ( p->Type == SsPartType::Instance )
		{
			if (sspj)
			{
				//参照アニメーションを取得
				int32 AnimPackIndex, AnimationIndex;
				if(sspj->FindAnimationIndex(p->RefAnimePack, p->RefAnime, AnimPackIndex, AnimationIndex))
				{
					FSsAnimePack* refpack = &sspj->AnimeList[AnimPackIndex];
					FSsAnimation* refanime = &refpack->AnimeList[AnimationIndex];

					FSsCellMapList* __cellmap = new FSsCellMapList();
					__cellmap->Set( sspj , refpack );
					FSsAnimeDecoder* animedecoder = new FSsAnimeDecoder();
					animedecoder->SetAnimation( &refpack->Model , refanime , __cellmap , sspj );
					PartState[i].RefAnime = animedecoder;
					//親子関係を付ける
					animedecoder->PartState[0].Parent = &PartState[i];
				}
			}
		}

		SortList.Add( &PartState[i] );

	}


	//アニメの最大フレーム数を取得
	CurAnimeEndFrame = anime->Settings.FrameCount;
	CurAnimeFPS = anime->Settings.Fps;

}


// パーツ名からインデックスを取得
int FSsAnimeDecoder::GetPartIndexFromName(FName PartName) const
{
	for(int i = 0; i < PartAnime.Num(); ++i)
	{
		if(PartAnime[i].Value->PartName == PartName)
		{
			return i;
		}
	}
	return -1;
}

// パーツのTransformを取得(Canvas内2D座標系)
FTransform FSsAnimeDecoder::GetPartTransformInCanvas(int PartIndex, int32 CanvasWidth, int32 CanvasHeight, const FSsAnimeDecoder::DrawOption& Option) const
{
	if((PartIndex < 0) ||(SortList.Num() <= PartIndex))
	{
		return FTransform();
	}

	FSsPartState* State = &(PartState[PartIndex]);
	float OffX = Option.CenterLocation.X;
	float OffY = CanvasHeight - Option.CenterLocation.Y;
	
	FVector2D PivotPosition(
		(State->Matrix[12] * Option.Scale.X) * (Option.bFlipH ? -1.f : 1.f) + OffX,
		CanvasHeight - (State->Matrix[13] * Option.Scale.Y) * (Option.bFlipV ? -1.f : 1.f) - OffY
		);

	float Rot = State->Rotation.Z;
	for(FSsPartState* ParentState = State->Parent; NULL != ParentState; ParentState = ParentState->Parent)
	{
		Rot += ParentState->Rotation.Z;
	}
	while(Rot <   0.f){ Rot += 360.f; }
	while(Rot > 360.f){ Rot -= 360.f; }
	if(Option.bFlipH)
	{
		Rot = 360.f - Rot;
	}
	if(Option.bFlipV)
	{
		if(Rot < 180.f){ Rot = 180.f - Rot; }
		else { Rot = 540.f - Rot; }
	}
	Rot += Option.Rotation;

	return FTransform(
		FRotator(0.f, 0.f, Rot),
		FVector(PivotPosition.X, PivotPosition.Y, 0.f),
		FVector(State->Scale.X, State->Scale.Y, 1.f)
		);
}



//頂点変形アニメデータの取得
void FSsAnimeDecoder::SsInterpolationValue(int time , const FSsKeyframe* leftkey , const FSsKeyframe* rightkey , FSsVertexAnime& v)
{
	//todo ロード時に FSsVertexAnimeを作成してしまうようにする
	FSsVertexAnime	lv;
	FSsVertexAnime	rv;

	if ( rightkey == 0 ) //右側が０出会った場合
	{
		GetFSsVertexAnime( leftkey , v );
		return ;
	}

	GetFSsVertexAnime(leftkey,lv);
	GetFSsVertexAnime(rightkey,rv);

	int range = rightkey->Time - leftkey->Time;
	float now = (float)(time - leftkey->Time) / range;

	FSsCurve curve;
	curve = leftkey->Curve;
	if (leftkey->IpType == SsInterpolationType::Bezier)
	{
		// ベジェのみキーの開始・終了時間が必要
		curve.StartKeyTime = leftkey->Time;
		curve.EndKeyTime = rightkey->Time;
	}

	float rate = SsInterpolate(leftkey->IpType, now, 0.f, 1.f, &curve);
	for ( int i = 0 ; i < 4 ; i++ )
	{
		v.Offsets[i].X = SsInterpolate(SsInterpolationType::Linear, rate, lv.Offsets[i].X, rv.Offsets[i].X, NULL);
		v.Offsets[i].Y = SsInterpolate(SsInterpolationType::Linear, rate, lv.Offsets[i].Y, rv.Offsets[i].Y, NULL);
	}

}


static float clamp(float v, float vmin, float vmax)
{
	float ret = v;
	if(v < vmin){ ret = vmin; }
	if(v > vmax){ ret = vmax; }
	return ret;
}

void	FSsAnimeDecoder::SsInterpolationValue( int time , const FSsKeyframe* leftkey , const FSsKeyframe* rightkey , FSsColorAnime& v )
{
	if ( rightkey == 0 )
	{
		GetSsColorValue( leftkey , v );
		return ;
	}
	
	FSsColorAnime leftv;
	FSsColorAnime rightv;

	GetSsColorValue( leftkey , leftv );
	GetSsColorValue( rightkey , rightv );


	FSsCurve curve;
	curve = leftkey->Curve;
	if (leftkey->IpType == SsInterpolationType::Bezier)
	{
		// ベジェのみキーの開始・終了時間が必要
		curve.StartKeyTime = leftkey->Time;
		curve.EndKeyTime = rightkey->Time;
	}

	int range = rightkey->Time - leftkey->Time;
	float now = (float)(time - leftkey->Time) / range;

	v.Color.Rgba.A = 0;
	v.Color.Rgba.R = 0;
	v.Color.Rgba.G = 0;
	v.Color.Rgba.B = 0;

	if(leftv.Target == SsColorBlendTarget::Vertex)
	{
		if(rightv.Target == SsColorBlendTarget::Vertex)
		{
			// 両方とも４頂点カラー
			for(int i = 0; i < 4; ++i)
			{
				v.Colors[i].Rate = clamp(SsInterpolate(SsInterpolationType::Linear, now, leftv.Colors[i].Rate, rightv.Colors[i].Rate, &curve), 0.f, 1.f);
				v.Colors[i].Rgba.A = clamp(SsInterpolate(SsInterpolationType::Linear, now, leftv.Colors[i].Rgba.A, rightv.Colors[i].Rgba.A, &curve), 0.f, 255.f);
				v.Colors[i].Rgba.R = clamp(SsInterpolate(SsInterpolationType::Linear, now, leftv.Colors[i].Rgba.R, rightv.Colors[i].Rgba.R, &curve), 0.f, 255.f);
				v.Colors[i].Rgba.G = clamp(SsInterpolate(SsInterpolationType::Linear, now, leftv.Colors[i].Rgba.G, rightv.Colors[i].Rgba.G, &curve), 0.f, 255.f);
				v.Colors[i].Rgba.B = clamp(SsInterpolate(SsInterpolationType::Linear, now, leftv.Colors[i].Rgba.B, rightv.Colors[i].Rgba.B, &curve), 0.f, 255.f);
				v.Target = SsColorBlendTarget::Vertex;
			}
		}
		else
		{
			// 左は４頂点、右は単色
			for(int i = 0; i < 4; ++i)
			{
				v.Colors[i].Rate = clamp(SsInterpolate(SsInterpolationType::Linear, now, leftv.Colors[i].Rate, rightv.Color.Rate, &curve), 0.f, 1.f);
				v.Colors[i].Rgba.A = clamp(SsInterpolate(SsInterpolationType::Linear, now, leftv.Colors[i].Rgba.A, rightv.Color.Rgba.A, &curve), 0.f, 255.f);
				v.Colors[i].Rgba.R = clamp(SsInterpolate(SsInterpolationType::Linear, now, leftv.Colors[i].Rgba.R, rightv.Color.Rgba.R, &curve), 0.f, 255.f);
				v.Colors[i].Rgba.G = clamp(SsInterpolate(SsInterpolationType::Linear, now, leftv.Colors[i].Rgba.G, rightv.Color.Rgba.G, &curve), 0.f, 255.f);
				v.Colors[i].Rgba.B = clamp(SsInterpolate(SsInterpolationType::Linear, now, leftv.Colors[i].Rgba.B, rightv.Color.Rgba.B, &curve), 0.f, 255.f);
				v.Target = SsColorBlendTarget::Vertex;
			}
		}
	}
	else
	{
		if(rightv.Target == SsColorBlendTarget::Vertex)
		{
			// 左は単色、右は４頂点カラー
			for(int i = 0; i < 4; ++i)
			{
				v.Colors[i].Rate = clamp(SsInterpolate(SsInterpolationType::Linear, now, leftv.Color.Rate, rightv.Colors[i].Rate, &curve), 0.f, 1.f);
				v.Colors[i].Rgba.A = clamp(SsInterpolate(SsInterpolationType::Linear, now, leftv.Color.Rgba.A, rightv.Colors[i].Rgba.A, &curve), 0.f, 255.f);
				v.Colors[i].Rgba.R = clamp(SsInterpolate(SsInterpolationType::Linear, now, leftv.Color.Rgba.R, rightv.Colors[i].Rgba.R, &curve), 0.f, 255.f);
				v.Colors[i].Rgba.G = clamp(SsInterpolate(SsInterpolationType::Linear, now, leftv.Color.Rgba.G, rightv.Colors[i].Rgba.G, &curve), 0.f, 255.f);
				v.Colors[i].Rgba.B = clamp(SsInterpolate(SsInterpolationType::Linear, now, leftv.Color.Rgba.B, rightv.Colors[i].Rgba.B, &curve), 0.f, 255.f);
				v.Target = SsColorBlendTarget::Vertex;
			}
		}
		else
		{
			// 両方とも単色
			v.Color.Rate = clamp(SsInterpolate(SsInterpolationType::Linear, now, leftv.Color.Rate, rightv.Color.Rate, &curve), 0.f, 1.f);
			v.Color.Rgba.A = clamp(SsInterpolate(SsInterpolationType::Linear, now, leftv.Color.Rgba.A, rightv.Color.Rgba.A, &curve), 0.f, 255.f);
			v.Color.Rgba.R = clamp(SsInterpolate(SsInterpolationType::Linear, now, leftv.Color.Rgba.R, rightv.Color.Rgba.R, &curve), 0.f, 255.f);
			v.Color.Rgba.G = clamp(SsInterpolate(SsInterpolationType::Linear, now, leftv.Color.Rgba.G, rightv.Color.Rgba.G, &curve), 0.f, 255.f);
			v.Color.Rgba.B = clamp(SsInterpolate(SsInterpolationType::Linear, now, leftv.Color.Rgba.B, rightv.Color.Rgba.B, &curve), 0.f, 255.f);
			v.Target = SsColorBlendTarget::Whole;
		}
	}
}

void	FSsAnimeDecoder::SsInterpolationValue( int time , const FSsKeyframe* leftkey , const FSsKeyframe* rightkey , FSsCellValue& v )
{
	FSsRefCell cell;
	GetFSsRefCell( leftkey , cell );

	FSsCelMapLinker* l = this->CurCellMapManager->GetCellMapLink( cell.Mapid );
	v.Cell = l->FindCell( cell.Name );

	v.FilterMode = l->CellMap->FilterMode;
	v.WrapMode = l->CellMap->WrapMode;

	if ( l->Tex )
	{
		v.Texture = l->Tex;
	}
	else
	{
		v.Texture = 0;
	}

	CalcUvs( &v );

}


void	FSsAnimeDecoder::SsInterpolationValue( int time , const FSsKeyframe* leftkey , const FSsKeyframe* rightkey , bool& v )
{
	if ( rightkey == 0 )
	{
		v = leftkey->Value.get<bool>();
		return ;
	}
	
	float v1 = (float)leftkey->Value.get<bool>();
	float v2 = (float)rightkey->Value.get<bool>();

	int range = rightkey->Time - leftkey->Time;
	float now = (float)(time - leftkey->Time) / range;

	if (leftkey->IpType == SsInterpolationType::Bezier)
	{
		// ベジェのみキーの開始・終了時間が必要
		FSsCurve curve;
		curve = leftkey->Curve;
		curve.StartKeyTime = leftkey->Time;
		curve.EndKeyTime = (float)rightkey->Time;
		v = 0.f != SsInterpolate( leftkey->IpType , now , v1 , v2 , &curve );
	}
	else{
		v = 0.f != SsInterpolate( leftkey->IpType , now , v1 , v2 , &leftkey->Curve );
	}
}

void FSsAnimeDecoder::SsInterpolationValue(int time, const FSsKeyframe* leftkey, const FSsKeyframe* rightkey, FSsInstanceAttr& v)
{
	//補間は行わないので、常に左のキーを出力する
	GetSsInstparamAnime(leftkey, v);
}

//float , int 基本型はこれで値の補間を行う
template<typename mytype>
void	FSsAnimeDecoder::SsInterpolationValue( int time , const FSsKeyframe* leftkey , const FSsKeyframe* rightkey , mytype& v )
{
	if ( rightkey == 0 )
	{
		v = leftkey->Value.get<mytype>();
		return ;
	}
	
	float v1 = (float)leftkey->Value.get<mytype>();
	float v2 = (float)rightkey->Value.get<mytype>();

	int range = rightkey->Time - leftkey->Time;
	float now = (float)(time - leftkey->Time) / range;

	if (leftkey->IpType == SsInterpolationType::Bezier)
	{
		// ベジェのみキーの開始・終了時間が必要
		FSsCurve curve;
		curve = leftkey->Curve;
		curve.StartKeyTime = leftkey->Time;
		curve.EndKeyTime = (float)rightkey->Time;
		v = SsInterpolate( leftkey->IpType , now , v1 , v2 , &curve );
	}
	else{
		v = SsInterpolate( leftkey->IpType , now , v1 , v2 , &leftkey->Curve );
	}
}


template<typename mytype> int	FSsAnimeDecoder::SsGetKeyValue( int time , FSsAttribute* attr , mytype&  value )
{
	int	useTime = 0;

	if ( attr->isEmpty() )
	{
		//デフォルト値を入れる まだ未実装
		return useTime;
	}

	const FSsKeyframe* lkey = attr->FindLeftKey( time );

	//無い場合は、最初のキーを採用する
	if ( lkey == 0 )
	{
		lkey =  attr->FirstKey();
		SsInterpolationValue( time , lkey , 0 , value );

		useTime = lkey->Time;
		return useTime;

	}else if ( lkey->Time == time )
	{
		SsInterpolationValue( time , lkey , 0 , value );
		useTime = time;
		return useTime;
	}else{
		//補間計算をする
		const FSsKeyframe* rkey = attr->FindRightKey( time );
		if (rkey == NULL)
		{
			// 次のキーが無いので補間できない。よって始点キーの値をそのまま返す
			SsInterpolationValue( time , lkey , 0 , value );
			useTime = lkey->Time;
			return useTime;
		}else
		if (lkey->IpType == SsInterpolationType::None)
		{
			// 補間なし指定なので始点キーの値…
			SsInterpolationValue( time , lkey , 0 , value );
			useTime = lkey->Time;
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
static void	CoordinateGetDiagonalIntersection( FVector2D& out , const FVector2D& LU , const FVector2D& RU , const FVector2D& LD , const FVector2D& RD )
{
	out = FVector2D(0.f,0.f);

	/* <<< 係数を求める >>> */
	float c1 = (LD.Y - RU.Y) * (LD.X - LU.X) - (LD.X - RU.X) * (LD.Y - LU.Y);
	float c2 = (RD.X - LU.X) * (LD.Y - LU.Y) - (RD.Y - LU.Y) * (LD.X - LU.X);
	float c3 = (RD.X - LU.X) * (LD.Y - RU.Y) - (RD.Y - LU.Y) * (LD.X - RU.X);


	if ( c3 <= 0 && c3 >=0) return;

	float ca = c1 / c3;
	float cb = c2 / c3;

	/* <<< 交差判定 >>> */
	if(((0.0f <= ca) && (1.0f >= ca)) && ((0.0f <= cb) && (1.0f >= cb)))
	{	/* 交差している */
		out.X = LU.X + ca * (RD.X - LU.X);
		out.Y = LU.Y + ca * (RD.Y - LU.Y);
	}
}

static FVector2D GetLocalScale( float matrix[16] )
{
	float sx = FVector2D::Distance( FVector2D( matrix[0] , matrix[1] ) , FVector2D( 0 , 0 ) );
	float sy = FVector2D::Distance( FVector2D( matrix[4 * 1 + 0] , matrix[4 * 1 + 1] ) , FVector2D( 0 , 0 ) );

	return FVector2D( sx , sy );
}


///現在の時間からパーツのアトリビュートの補間値を計算する
void	FSsAnimeDecoder::UpdateState( int nowTime , FSsPart* part , FSsPartAnime* anime , FSsPartState* state )
{

	//ステートの初期値を設定
	state->Init();
	state->InheritRates = part->InheritRates;
	if ( anime == 0 ){
		IdentityMatrix( state->Matrix );
		return ;
	}

	// 親の継承設定を引用する設定の場合、ここで参照先を親のものに変えておく。
	if (part->InheritType == SsInheritType::Parent)
	{
		if ( state->Parent )
		{
			state->InheritRates = state->Parent->InheritRates;
		}
	}

	bool	size_x_key_find = false;
	bool	size_y_key_find = false;

	state->IsVertexTransform = false;
	state->IsColorBlend = false;
	state->AlphaBlendType = part->AlphaBlendType;


	bool hidekey_find = false;

	if ( 0 < anime->Attributes.Num() )
	{
		for(int i = 0; i < anime->Attributes.Num(); ++i)
		{
			FSsAttribute* attr = &(anime->Attributes[i]);
			switch( attr->Tag )
			{
				case SsAttributeKind::Invalid:	///< 無効値。旧データからの変換時など
					break;
				case SsAttributeKind::Cell:		///< 参照セル
					{
						SsGetKeyValue( nowTime , attr , state->CellValue );
						state->NoCells = false;
					}
					break;
				case SsAttributeKind::Posx:		///< 位置.X
					SsGetKeyValue( nowTime , attr , state->Position.X );
					break;
				case SsAttributeKind::Posy:		///< 位置.Y
					SsGetKeyValue( nowTime , attr , state->Position.Y );
					break;
				case SsAttributeKind::Posz:		///< 位置.Z
					SsGetKeyValue( nowTime , attr , state->Position.Z );
					break;
				case SsAttributeKind::Rotx:		///< 回転.X
					SsGetKeyValue( nowTime , attr , state->Rotation.X );
					break;
				case SsAttributeKind::Roty:		///< 回転.Y
					SsGetKeyValue( nowTime , attr , state->Rotation.Y );
					break;
				case SsAttributeKind::Rotz:		///< 回転.Z
					SsGetKeyValue( nowTime , attr , state->Rotation.Z );
					break;
				case SsAttributeKind::Sclx:		///< スケール.X
					SsGetKeyValue( nowTime , attr , state->Scale.X );
					break;
				case SsAttributeKind::Scly:		///< スケール.Y
					SsGetKeyValue( nowTime , attr , state->Scale.Y );
					break;
				case SsAttributeKind::Alpha:	///< 不透明度
					SsGetKeyValue( nowTime , attr , state->Alpha );
					break;
				case SsAttributeKind::Prio:		///< 優先度
					SsGetKeyValue( nowTime , attr , state->Prio );
					break;
				case SsAttributeKind::Fliph:	///< 左右反転(セルの原点を軸にする)
					SsGetKeyValue( nowTime , attr , state->HFlip );
					break;
				case SsAttributeKind::Flipv:	///< 上下反転(セルの原点を軸にする)
					SsGetKeyValue( nowTime , attr , state->VFlip );
					break;
				case SsAttributeKind::Hide:		///< 非表示
					{
						int useTime = SsGetKeyValue( nowTime , attr , state->Hide );
						// 非表示キーがないか、先頭の非表示キーより手前の場合は常に非表示にする。
						if ( useTime > nowTime )
						{
							state->Hide = true;
						}
						// 非表示キーがあり、かつ最初のキーフレームを取得した
						else
						{
							hidekey_find = true;
						}
					}
					break;
				case SsAttributeKind::Color:	///< カラーブレンド
					SsGetKeyValue( nowTime , attr , state->ColorValue );
					state->IsColorBlend = true;
					break;
				case SsAttributeKind::Vertex:	///< 頂点変形
					SsGetKeyValue( nowTime , attr , state->VertexValue );
					state->IsVertexTransform = true;
					break;
				case SsAttributeKind::Pivotx:	///< 原点オフセット.X
					SsGetKeyValue( nowTime , attr , state->PivotOffset.X );
					break;
				case SsAttributeKind::Pivoty:	///< 原点オフセット.Y
					SsGetKeyValue( nowTime , attr , state->PivotOffset.Y );
					break;
				case SsAttributeKind::Anchorx:	///< アンカーポイント.X
					SsGetKeyValue( nowTime , attr , state->Anchor.X );
					break;
				case SsAttributeKind::Anchory:	///< アンカーポイント.Y
					SsGetKeyValue( nowTime , attr , state->Anchor.Y );
					break;
				case SsAttributeKind::Sizex:	///< 表示サイズ.X
					SsGetKeyValue( nowTime , attr , state->Size.X );
					size_x_key_find = true;
					break;
				case SsAttributeKind::Sizey:	///< 表示サイズ.Y
					SsGetKeyValue( nowTime , attr , state->Size.Y );
					size_y_key_find = true;
					break;
				case SsAttributeKind::Imgfliph:	///< イメージ左右反転(常にイメージの中央を原点とする)
					SsGetKeyValue( nowTime , attr , state->ImageFlipH );
					break;
				case SsAttributeKind::Imgflipv:	///< イメージ上下反転(常にイメージの中央を原点とする)
					SsGetKeyValue( nowTime , attr , state->ImageFlipV );
					break;
				case SsAttributeKind::Uvtx:		///< UVアニメ.移動.X
					SsGetKeyValue( nowTime , attr , state->UvTranslate.X );
					break;
				case SsAttributeKind::Uvty:		///< UVアニメ.移動.Y
					SsGetKeyValue( nowTime , attr , state->UvTranslate.Y );
					break;
				case SsAttributeKind::Uvrz:		///< UVアニメ.回転
					SsGetKeyValue( nowTime , attr , state->UvRotation );
					break;
				case SsAttributeKind::Uvsx:		///< UVアニメ.スケール.X
					SsGetKeyValue( nowTime , attr , state->UvScale.X );
					break;
				case SsAttributeKind::Uvsy:		///< UVアニメ.スケール.Y
					SsGetKeyValue( nowTime , attr , state->UvScale.Y );
					break;
				case SsAttributeKind::Boundr:	///< 当たり判定用の半径
					SsGetKeyValue(nowTime, attr, state->BoundingRadius);
					break;
				case SsAttributeKind::User:		///< Ver.4 互換ユーザーデータ
					break;
				case SsAttributeKind::Instance:	///インスタンスパラメータ
					SsGetKeyValue(nowTime, attr, state->InstanceValue);
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
		if (state->NoCells)
			state->Colors[i] = sColorsForBoundsParts[i & 3];
		else
			state->Colors[i] = 1.f;
	}

	// 継承
	if (state->Parent)
	{
		// α
		if (state->Inherits_(SsAttributeKind::Alpha))
		{
			state->Alpha *= state->Parent->Alpha;
		}

		// フリップの継承。継承ONだと親に対しての反転になる…ヤヤコシス
		if (state->Inherits_(SsAttributeKind::Fliph))
		{
			state->HFlip = state->Parent->HFlip ^ state->HFlip;
		}
		if (state->Inherits_(SsAttributeKind::Flipv))
		{
			state->VFlip = state->Parent->VFlip ^ state->VFlip;
		}

		// 引き継ぐ場合は親の値をそのまま引き継ぐ
		if (state->Inherits_(SsAttributeKind::Hide))
		{
			state->Hide = state->Parent->Hide;
		}
	}

	// 非表示キーがないか、先頭の非表示キーより手前の場合は常に非表示にする。(継承関係なし)
	if (!hidekey_find)
	{
		state->Hide = true;
	}

	// 頂点の設定
	if ( part->Type == SsPartType::Normal )
	{
		FSsCell * cell = state->CellValue.Cell;
		if (cell && anime)
		{
			//サイズアトリビュートが指定されていない場合、セルのサイズを設定する
			if ( !size_x_key_find ) state->Size.X = cell->Size.X;
			if ( !size_y_key_find ) state->Size.Y = cell->Size.Y;
		}
		UpdateVertices(part , anime , state);
	}



}

void	FSsAnimeDecoder::UpdateMatrix(FSsPart* part , FSsPartAnime* anime , FSsPartState* state)
{

	IdentityMatrix( state->Matrix );

	if (state->Parent)
	{
		memcpy( state->Matrix , state->Parent->Matrix , sizeof( float ) * 16 );
	}

	// アンカー
	if ( state->Parent )
	{
		const FVector2D& parentSize = state->Parent->Size;
		state->Position.X = state->Position.X + state->Parent->Size.X * state->Anchor.X;
		state->Position.Y = state->Position.Y + state->Parent->Size.Y * state->Anchor.Y;
	}

	TranslationMatrixM( state->Matrix , state->Position.X, state->Position.Y, state->Position.Z );//
	RotationXYZMatrixM( state->Matrix , DegreeToRadian(state->Rotation.X) , DegreeToRadian(state->Rotation.Y) , DegreeToRadian( state->Rotation.Z) );
	ScaleMatrixM(  state->Matrix , state->Scale.X, state->Scale.Y, 1.0f );
}


void	FSsAnimeDecoder::UpdateVertices(FSsPart* part , FSsPartAnime* anime , FSsPartState* state)
{

	FSsCell * cell = state->CellValue.Cell;

	FVector2D pivot;
	if (cell)
	{
		// セルに設定された原点オフセットを適用する
		// ※セルの原点は中央が0,0で＋が右上方向になっている
		float cpx = cell->Pivot.X + 0.5f;
		if (state->HFlip) cpx = 1 - cpx;	// 水平フリップによって原点を入れ替える
		pivot.X = cpx * state->Size.X;
		// 上が＋で入っているのでここで反転する。
		float cpy = -cell->Pivot.Y + 0.5f;
		if (state->VFlip) cpy = 1 - cpy;	// 垂直フリップによって原点を入れ替える
		pivot.Y = cpy * state->Size.Y;
	}
	else
	{
		// セルが無いパーツでも原点が中央に来るようにする。
		pivot.X = 0.5f * state->Size.X;
		pivot.Y = 0.5f * state->Size.Y;
	}

	// 次に原点オフセットアニメの値を足す
	pivot.X += state->PivotOffset.X * state->Size.X;
	pivot.Y += -state->PivotOffset.Y * state->Size.Y;

	float sx = -pivot.X;
	float ex = sx + state->Size.X;
	float sy = +pivot.Y;
	float ey = sy - state->Size.Y;

	// Z順
	/*
		これは実は上下ひっくり返って裏面になっているためUV値も上下反転させている。
		左上が最初に来る並びの方が頂点カラー・頂点変形のデータと同じで判りやすいのでこれでいく。
	*/
	float vtxPosX[4] = {sx, ex, sx, ex};
	float vtxPosY[4] = {sy, sy, ey, ey};

	FVector2D * vtxOfs = state->VertexValue.Offsets;

	//きれいな頂点変形に対応
#if USE_TRIANGLE_FIN

	if ( state->is_color_blend || state->is_vertex_transform )
	{

		FVector2D	vertexCoordinateLU = FVector2D( sx + (float)vtxOfs[0].x , sy + (float)vtxOfs[0].y );// : 左上頂点座標（ピクセル座標系）
		FVector2D	vertexCoordinateRU = FVector2D( ex + (float)vtxOfs[1].x , sy + (float)vtxOfs[1].y );// : 右上頂点座標（ピクセル座標系）
		FVector2D	vertexCoordinateLD = FVector2D( sx + (float)vtxOfs[2].x , ey + (float)vtxOfs[2].y );// : 左下頂点座標（ピクセル座標系）
		FVector2D	vertexCoordinateRD = FVector2D( ex + (float)vtxOfs[3].x , ey + (float)vtxOfs[3].y );// : 右下頂点座標（ピクセル座標系）

		FVector2D CoordinateLURU = (vertexCoordinateLU + vertexCoordinateRU) * 0.5f;
		FVector2D CoordinateLULD = (vertexCoordinateLU + vertexCoordinateLD) * 0.5f;
		FVector2D CoordinateLDRD = (vertexCoordinateLD + vertexCoordinateRD) * 0.5f;
		FVector2D CoordinateRURD = (vertexCoordinateRU + vertexCoordinateRD) * 0.5f;

		FVector2D center;
		CoordinateGetDiagonalIntersection( center , CoordinateLURU, CoordinateRURD, CoordinateLULD, CoordinateLDRD );

		FVector2D*	coodinatetable[] = { &vertexCoordinateLU , &vertexCoordinateRU , &vertexCoordinateLD , &vertexCoordinateRD , &center };


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
	//FVector2D * vtxOfs = vertexValue.offsets;
	for (int i = 0; i < 4; ++i)
	{
		state->Vertices[i * 3]		= vtxPosX[i] + (float)vtxOfs->X;
		state->Vertices[i * 3 + 1]	= vtxPosY[i] + (float)vtxOfs->Y;
		state->Vertices[i * 3 + 2]	= 0;

		++vtxOfs;
	}
#endif



}



void	FSsAnimeDecoder::UpdateInstance( int nowTime , FSsPart* part , FSsPartAnime* partanime , FSsPartState* state )
{
	if ( state->RefAnime == 0 ){ return; }
	
	FSsAnimation* anime = state->RefAnime->CurAnimation;
	const FSsInstanceAttr& instanceValue = state->InstanceValue;

	//プレイヤー等では再生開始時にいったん計算してしまって値にしてしまった方がいい。
	//エディター側のロジックなのでそのまま検索する
	//インスタンスアニメ内のスタート位置
    int	startframe = CalcAnimeLabel2Frame(instanceValue.StartLabel, instanceValue.StartOffset, anime);
    int	endframe = CalcAnimeLabel2Frame(instanceValue.EndLabel, instanceValue.EndOffset, anime);

    state->InstanceValue.StartFrame = startframe;		//ラベル位置とオフセット位置を加えた実際のフレーム数
    state->InstanceValue.EndFrame = endframe;			//ラベル位置とオフセット位置を加えた実際のフレーム数

    //タイムライン上の時間 （絶対時間）
	int time = nowTime;

	//独立動作の場合
	if ( instanceValue.Independent )
	{
		//float delta = animeState->frame - parentBackTime;
		float delta = this->FrameDelta;

		state->InstanceValue.LiveFrame+= ( delta * instanceValue.Speed );
		//parentBackTime = animeState->frame;
		time = (int)instanceValue.LiveFrame;

	}

    //このインスタンスが配置されたキーフレーム（絶対時間）
    int	selfTopKeyframe = instanceValue.CurKeyframe;


    int	reftime = (time*instanceValue.Speed) - selfTopKeyframe; //開始から現在の経過時間
    if ( reftime < 0 ) return ; //そもそも生存時間に存在していない

    int inst_scale = (endframe - startframe) + 1; //インスタンスの尺


	//尺が０もしくはマイナス（あり得ない
	if ( inst_scale <= 0 ) return ;

	int	nowloop =  (reftime / inst_scale);	//現在までのループ数

    int checkloopnum = instanceValue.LoopNum;

	//pingpongの場合では２倍にする
    if ( instanceValue.Pingpong ) checkloopnum = checkloopnum * 2;

	//無限ループで無い時にループ数をチェック
    if ( !instanceValue.Infinity )   //無限フラグが有効な場合はチェックせず
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
	bool reverse = instanceValue.Reverse;
	if(instanceValue.Pingpong && (nowloop % 2 == 1))
	{
		if(reverse)
		{
			reverse = false;
		}
		else
		{
			reverse = true;
		}
	}

	if (reverse)
	{
		//リバースの時
		_time = endframe - temp_frame;
	}else{
		//通常時
		_time = temp_frame + startframe;
	}

	state->RefAnime->SetPlayFrame( _time );
	state->RefAnime->Update();
}

int		FSsAnimeDecoder::FindAnimetionLabel(const FName& str, FSsAnimation* Animation)
{
	for(int i = 0; i < Animation->Labels.Num(); ++i)
	{
		if ( str == Animation->Labels[i].LabelName )
		{
			return Animation->Labels[i].Time;
		}
	}

	return 0;
}

int		FSsAnimeDecoder::CalcAnimeLabel2Frame(const FName& str, int offset, FSsAnimation* Animation)
{

	//10フレームのアニメだと11が入ってるため計算がずれるため-1する
	int maxframe = Animation->Settings.FrameCount - 1;
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
		int ret = FindAnimetionLabel(str, Animation);

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

///SS5の場合  SsPartのarrayIndexは、親子順　（子は親より先にいない）と
///なっているためそのまま木構造を作らずUpdateを行う
void FSsAnimeDecoder::Update()
{
	int	time = (int)NowPlatTime;

	for(int i = 0; i < PartAnime.Num(); ++i)
	{
		FSsPart* part = PartAnime[i].Key;
		FSsPartAnime* anime = PartAnime[i].Value;
		UpdateState(time , part , anime , &PartState[i]);
		UpdateMatrix(part , anime , &PartState[i]);

		if(part->Type == SsPartType::Instance)
		{
			UpdateInstance(time , part , anime , &PartState[i]);
			UpdateVertices(part , anime , &PartState[i]);
		}
	}

	SortList.Sort();

}



// 描画
void FSsAnimeDecoder::Draw(FCanvas* Canvas, const FSsAnimeDecoder::DrawOption& Option)
{
	if(NULL == Canvas)
	{
		return;
	}

	for(int i = 0; i < SortList.Num(); ++i)
	{
		FSsPartState* State = SortList[i];

		if ( State->RefAnime )
		{
			if(!State->Hide)
			{
				State->RefAnime->Draw(Canvas, Option);
			}
		}
		else
		{
			DrawPart(Canvas, State, Option);
		}
	}
}

namespace
{
	inline float BlendColorValue(SsBlendType::Type BlendType, uint8 color8, float rate)
	{
		float c = static_cast<float>(color8) / 255.f;

		// ホントはシェーダで実装する. 今はとりあえず...
		switch(BlendType)
		{
			case SsBlendType::Mix:
				{} break;
			case SsBlendType::Mul:	// 現状で比較的まともに反映されるのはコレだけ
				{
					c = c * rate + (1.f - rate);
				} break;
			case SsBlendType::Add:
			case SsBlendType::Sub:
				{
					c = c * rate;
				} break;
		}
		return c;
	}
	inline void BlendColor(FLinearColor& Out, SsBlendType::Type BlendType, const FSsColorBlendValue& Color)
	{
		Out.R = BlendColorValue(BlendType, Color.Rgba.R, Color.Rate);
		Out.G = BlendColorValue(BlendType, Color.Rgba.G, Color.Rate);
		Out.B = BlendColorValue(BlendType, Color.Rgba.B, Color.Rate);
	}
}

// パーツ単位での描画
void FSsAnimeDecoder::DrawPart(FCanvas* Canvas, FSsPartState* State, const FSsAnimeDecoder::DrawOption& Option)
{
	// 各種非表示チェック
	if(State->Hide){ return; }
	if(State->NoCells){ return; }
	if(NULL == State->CellValue.Cell){ return; }
	if(NULL == State->CellValue.Texture){ return; }

	// Canvasに対する描画基準位置
	FIntPoint CanvasSize = Canvas->GetRenderTarget()->GetSizeXY();
	float OffX = Option.CenterLocation.X;
	float OffY = Option.CenterLocation.Y;

	// 頂点座標
	FMatrix ViewMatrix(
		FVector(State->Matrix[ 0], State->Matrix[ 1], State->Matrix[ 2]),
		FVector(State->Matrix[ 4], State->Matrix[ 5], State->Matrix[ 6]),
		FVector(State->Matrix[ 8], State->Matrix[ 9], State->Matrix[10]),
		FVector(State->Matrix[12], State->Matrix[13], State->Matrix[14])
		);
	FVector2D Vertices2D[5];
	FLinearColor VertexColors[5];
	float R = DegreeToRadian(Option.Rotation);
	float SinR = FMath::Sin(R);
	float CosR = FMath::Cos(R);
	for(int i = 0; i < 5; ++i)
	{
		FVector4 V = ViewMatrix.TransformPosition(FVector(
			State->Vertices[i*3 + 0],
			State->Vertices[i*3 + 1],
			State->Vertices[i*3 + 2]
			));
		if(Option.bFlipH)
		{
			V.X *= -1.f;
		}
		if(Option.bFlipV)
		{
			V.Y *= -1.f;
		}
		if(0.f != Option.Rotation)
		{
			V = FVector4(V.X * CosR - V.Y * SinR, V.X * SinR + V.Y * CosR, V.Z, V.W);
		}
		Vertices2D[i] = FVector2D((V.X * Option.Scale.X) + OffX, (-V.Y * Option.Scale.Y) + OffY);
	}

	// 上下反転，左右反転
	if(State->HFlip)
	{
		FVector2D tmp;
		tmp = Vertices2D[0];
		Vertices2D[0] = Vertices2D[1];
		Vertices2D[1] = tmp;
		tmp = Vertices2D[2];
		Vertices2D[2] = Vertices2D[3];
		Vertices2D[3] = tmp;
	}
	if(State->VFlip)
	{
		FVector2D tmp;
		tmp = Vertices2D[0];
		Vertices2D[0] = Vertices2D[2];
		Vertices2D[2] = tmp;
		tmp = Vertices2D[1];
		Vertices2D[1] = Vertices2D[3];
		Vertices2D[3] = tmp;
	}

	// UV
	FVector2D UVs[5];
	for(int i = 0; i < 5; ++i)
	{
		UVs[i] = FVector2D(State->CellValue.Uvs[i].X + State->Uvs[i*2 + 0] + State->UvTranslate.X, State->CellValue.Uvs[i].Y + State->Uvs[i*2 + 1] + State->UvTranslate.Y);
	}
	if(1.f != State->UvScale.X)
	{
		float Center;
		Center = (UVs[1].X - UVs[0].X) / 2.f + UVs[0].X;
		UVs[0].X = Center - ((Center - UVs[0].X) * State->UvScale.X);
		UVs[1].X = Center - ((Center - UVs[1].X) * State->UvScale.X);
		Center = (UVs[3].X - UVs[2].X) / 2.f + UVs[2].X;
		UVs[2].X = Center - ((Center - UVs[2].X) * State->UvScale.X);
		UVs[3].X = Center - ((Center - UVs[3].X) * State->UvScale.X);
	}
	if(0.f != State->UvRotation)
	{
		FVector2D UVCenter((UVs[1].X - UVs[0].X) / 2.f + UVs[0].X, (UVs[2].Y - UVs[0].Y) / 2.f + UVs[0].Y);
		float S = FMath::Sin(DegreeToRadian(State->UvRotation));
		float C = FMath::Cos(DegreeToRadian(State->UvRotation));
		for(int i = 0; i < 4; ++i)
		{
			UVs[i] -= UVCenter;
			UVs[i] = FVector2D(
				UVs[i].X * C - UVs[i].Y * S,
				UVs[i].X * S + UVs[i].Y * C
				);
			UVs[i] += UVCenter;
		}
	}
	if(1.f != State->UvScale.Y)
	{
		float Center;
		Center = (UVs[2].Y - UVs[0].Y) / 2.f + UVs[0].Y;
		UVs[0].Y = Center - ((Center - UVs[0].Y) * State->UvScale.Y);
		UVs[2].Y = Center - ((Center - UVs[2].Y) * State->UvScale.Y);
		Center = (UVs[3].Y - UVs[1].Y) / 2.f + UVs[1].Y;
		UVs[1].Y = Center - ((Center - UVs[1].Y) * State->UvScale.Y);
		UVs[3].Y = Center - ((Center - UVs[3].Y) * State->UvScale.Y);
	}

	// イメージ反転
	if(State->ImageFlipH)
	{
		FVector2D tmp;
		tmp = UVs[0];
		UVs[0] = UVs[1];
		UVs[1] = tmp;
		tmp = UVs[2];
		UVs[2] = UVs[3];
		UVs[3] = tmp;
	}
	if(State->ImageFlipV)
	{
		FVector2D tmp;
		tmp = UVs[0];
		UVs[0] = UVs[2];
		UVs[2] = tmp;
		tmp = UVs[1];
		UVs[1] = UVs[3];
		UVs[3] = tmp;
	}

	// 頂点カラー
	if(State->IsColorBlend)
	{
		if(State->ColorValue.Target == SsColorBlendTarget::Whole)
		{
			const FSsColorBlendValue& cbv = State->ColorValue.Color;
			BlendColor(VertexColors[0], State->ColorValue.BlendType, cbv);
			VertexColors[0].A = BlendColorValue(SsBlendType::Mul, cbv.Rgba.A * State->Alpha, cbv.Rate);

			for(int32 i = 1; i < 5; ++i)
			{
				VertexColors[i] = VertexColors[0];
			}
		}
		else
		{
			for(int32 i = 0; i < 5; ++i)
			{
				const FSsColorBlendValue& cbv = State->ColorValue.Colors[i];
				BlendColor(VertexColors[i], State->ColorValue.BlendType, cbv);
				VertexColors[i].A = BlendColorValue(SsBlendType::Mul, cbv.Rgba.A * State->Alpha, cbv.Rate);
			}
		}
	}
	else
	{
		for(int32 i = 0; i < 5; ++i)
		{
			VertexColors[i] = FLinearColor(1.f, 1.f, 1.f, State->Alpha);
		}
	}


	// 描画
	UTexture* Texture = State->CellValue.Texture;
	if(Option.TextureReplacements)
	{
		TWeakObjectPtr<UTexture>* ReplaceTexture = Option.TextureReplacements->Find(State->Index);
		if(ReplaceTexture && ReplaceTexture->IsValid())
		{
			Texture = ReplaceTexture->Get();
		}
	}
#if defined(SS_SWITCH_VERTICAL_AXIS)
	for(int32 i = 0; i < 5; ++i)
	{
		Vertices2D[i].Y = CanvasSize.Y - Vertices2D[i].Y;
	}
#endif
	{
		FCanvasTriangleItem Tri(Vertices2D[0], Vertices2D[1], Vertices2D[3], UVs[0], UVs[1], UVs[3], Texture->Resource);
		Tri.BlendMode = Option.bDrawMask ? SE_BLEND_AlphaComposite : SE_BLEND_AlphaBlend;
		Tri.TriangleList[0].V0_Color = VertexColors[0];
		Tri.TriangleList[0].V1_Color = VertexColors[1];
		Tri.TriangleList[0].V2_Color = VertexColors[3];
		Canvas->DrawItem(Tri);
	}
	{
		FCanvasTriangleItem Tri(Vertices2D[0], Vertices2D[3], Vertices2D[2], UVs[0], UVs[3], UVs[2], Texture->Resource);
		Tri.BlendMode = Option.bDrawMask ? SE_BLEND_AlphaComposite : SE_BLEND_AlphaBlend;
		Tri.TriangleList[0].V0_Color = VertexColors[0];
		Tri.TriangleList[0].V1_Color = VertexColors[3];
		Tri.TriangleList[0].V2_Color = VertexColors[2];
		Canvas->DrawItem(Tri);
	}
}
