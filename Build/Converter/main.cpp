//
//  main.cpp
//  Converter
//
//  Created by Hiroki Azumada on 2014/01/07.
//  Copyright (c) 2014年 Hiroki Azumada. All rights reserved.
//

// TODO
#include "ssloader.h"
#include "babel/babel.h"
#include "ssplayer_animedecode.h"
#include "ssHelper.h"
#include "sshTextureBMP.h"

#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>

#ifndef _WIN32
#include <sys/stat.h>
#endif

#include "Lump.h"
#include "LumpExporter.h"
#include "FileUtil.h"
#include "SsPlayerConverter.h"


static const int DATA_VERSION_1			= 1;

static const int DATA_ID				= 0x42505353;
static const int CURRENT_DATA_VERSION	= DATA_VERSION_1;


enum {
	PART_FLAG_INVISIBLE			= 1 << 0,
	PART_FLAG_FLIP_H			= 1 << 2,
	PART_FLAG_FLIP_V			= 1 << 3,

	// optional parameter flags
	PART_FLAG_CELL_INDEX		= 1 << 4,
	PART_FLAG_POSITION_X		= 1 << 5,
	PART_FLAG_POSITION_Y		= 1 << 6,
	PART_FLAG_ANCHOR_X			= 1 << 7,
	PART_FLAG_ANCHOR_Y			= 1 << 8,
	PART_FLAG_ROTATION			= 1 << 9,
	PART_FLAG_SCALE_X			= 1 << 10,
	PART_FLAG_SCALE_Y			= 1 << 11,
	PART_FLAG_OPACITY			= 1 << 12,
	PART_FLAG_COLOR_BLEND		= 1 << 13,
	PART_FLAG_VERTEX_TRANSFORM	= 1 << 14,

	PART_FLAG_SIZE_X			= 1 << 15,
	PART_FLAG_SIZE_Y			= 1 << 16,

	PART_FLAG_U_MOVE			= 1 << 17,
	PART_FLAG_V_MOVE			= 1 << 18,
	PART_FLAG_UV_ROTATION		= 1 << 19,
	PART_FLAG_U_SCALE			= 1 << 20,
	PART_FLAG_V_SCALE			= 1 << 21,

	PART_FLAG_INSTANCE_KEYFRAME	= 1 << 22,
	PART_FLAG_INSTANCE_START	= 1 << 23,
	PART_FLAG_INSTANCE_END		= 1 << 24,
	PART_FLAG_INSTANCE_SPEED	= 1 << 25,
	PART_FLAG_INSTANCE_LOOP		= 1 << 26,
	PART_FLAG_INSTANCE_LOOP_FLG	= 1 << 27,

	NUM_PART_FLAGS
};

enum {
	VERTEX_FLAG_LT		= 1 << 0,
	VERTEX_FLAG_RT		= 1 << 1,
	VERTEX_FLAG_LB		= 1 << 2,
	VERTEX_FLAG_RB		= 1 << 3,
	VERTEX_FLAG_ONE		= 1 << 4	// color blend only
};

enum {
	INSTANCE_LOOP_FLAG_INFINITY		= 1 << 0,
	INSTANCE_LOOP_FLAG_REVERSE		= 1 << 1,
	INSTANCE_LOOP_FLAG_PINGPONG		= 1 << 2,
	INSTANCE_LOOP_FLAG_INDEPENDENT	= 1 << 3,
};

enum {
	USER_DATA_FLAG_INTEGER	= 1 << 0,
	USER_DATA_FLAG_RECT		= 1 << 1,
	USER_DATA_FLAG_POINT	= 1 << 2,
	USER_DATA_FLAG_STRING	= 1 << 3
};

//座標を固定少数で出力　100＝1ドット
#define DOT ( 100.0f )


bool convert_error_exit = false;	//データにエラーがありコンバートを中止した





typedef std::map<const SsCell*, int> CellList;

CellList* makeCellList(SsProject* proj)
{
	// セルデータの出力と、全てセルデータを集約したリストを作る
	CellList* cellList = new std::map<const SsCell*, int>();
	int cellListIndex = 0;

	for (size_t mapIndex = 0; mapIndex < proj->cellmapList.size(); mapIndex++)
	{
		const SsCellMap* cellMap = proj->cellmapList[mapIndex];

		for (size_t cellIndex = 0; cellIndex < cellMap->cells.size(); cellIndex++)
		{
			const SsCell* cell = cellMap->cells[cellIndex];
			cellList->insert(CellList::value_type(cell, cellListIndex++));
		}
	}
	
	return cellList;
}


static const SsKeyframe* findDefaultKeyframe(SsAnimeDecoder& decoder, int partIndex, SsAttributeKind::_enum tag)
{
	foreach(std::vector<SsPartAndAnime>, decoder.getPartAnime(), it)
	{
		SsPartAnime* partAnime = it->second;
		SsPart* part = it->first;
		if (part->arrayIndex != partIndex) continue;
		
		foreach(SsAttributeList, partAnime->attributes, attrIt)
		{
			SsAttribute* attr = *attrIt;
			if (attr->tag != tag) continue;
			
			const SsKeyframe* key = attr->firstKey();
			return key;
		}
		
	}
	return NULL;
}


static SsAttribute* findAttribute(SsPartAnime* partAnime, SsAttributeKind::_enum tag)
{
	foreach(SsAttributeList, partAnime->attributes, attrIt)
	{
		SsAttribute* attr = *attrIt;
		if (attr->tag == tag) return attr;
	}
	return NULL;
}


static const SsKeyframe* findFirstKey(SsPartAnime* partAnime, SsAttributeKind::_enum tag)
{
	SsAttribute* attr = findAttribute(partAnime, tag);
	if (attr)
	{
		return attr->firstKey();
	}
	return NULL;
}


static const SsPartState* findState(std::list<SsPartState*>& partList, int partIndex)
{
	foreach (std::list<SsPartState*>, partList, it)
	{
		const SsPartState* state = *it;
		if (state->index == partIndex) return state;
	}
	return NULL;
}




struct PartInitialData
{
	int		sortedOrder;
	int		index;
	int		flags;
	int		cellIndex;
	int		posX;
	int		posY;
	float	anchorX;
	float	anchorY;
	float	rotation;
	float	scaleX;
	float	scaleY;
	int		opacity;
	float	size_X;
	float	size_Y;
	float	uv_move_X;
	float	uv_move_Y;
	float	uv_rotation;
	float	uv_scale_X;
	float	uv_scale_Y;
};



//全全角が使われてるかのチェック
bool isZenkaku( const SsString* str )
{
	bool rc = false;
	int i = 0;
	int size = str->length();
	const char *c = str->c_str();

	while ( true )
	{
		if ( ( c[i] == '\0' ) || ( size == i ) )
		{
			//終了
			break;
		}
		if ( ( c[i] < 0x20 ) || ( c[i] > 0x7e ) )
		{
			//半角以外
			rc = true;
			//終了
			break;
		}
		i++;
	}
	return( rc );
}

static Lump* parseParts(SsProject* proj, const std::string& imageBaseDir)
{
//	static SsPartStateLess _ssPartStateLess;

	CellList* cellList = makeCellList(proj);

	Lump* topLump = Lump::set("ss::ProjectData", true);

	topLump->add(Lump::s32Data(DATA_ID));
	topLump->add(Lump::s32Data(CURRENT_DATA_VERSION));
	topLump->add(Lump::s32Data(0));
	
	if (imageBaseDir.length() > 0)
	{
		topLump->add(Lump::stringData(imageBaseDir));
	}
	else
	{
		topLump->add(Lump::s32Data(0));
	}

	Lump* cellsData = Lump::set("ss::Cell[]", true);
	topLump->add(cellsData);
	Lump* packDataArray = Lump::set("ss::AnimePackData[]", true);
	topLump->add(packDataArray);
	topLump->add(Lump::s16Data((int)cellList->size()));
	topLump->add(Lump::s16Data((int)proj->animeList.size()));

	// セルの情報
	for (size_t mapIndex = 0; mapIndex < proj->cellmapList.size(); mapIndex++)
	{
		const SsCellMap* cellMap = proj->cellmapList[mapIndex];
		
		Lump* cellMapData = Lump::set("ss::CellMap", true);
		cellMapData->add(Lump::stringData(cellMap->name));
		cellMapData->add(Lump::stringData(cellMap->imagePath));
		cellMapData->add(Lump::s16Data((int)mapIndex));
		cellMapData->add(Lump::s16Data(0));	// reserved

		//全角チェック
		if ( isZenkaku( &cellMap->name ) == true )
		{
			std::cerr << "エラー：全角が使用されている: " << cellMap->name << "\n";
			convert_error_exit = true;	//エラーが発生コンバート失敗
		}

		
		for (size_t cellIndex = 0; cellIndex < cellMap->cells.size(); cellIndex++)
		{
			const SsCell* cell = cellMap->cells[cellIndex];

			Lump* cellData = Lump::set("ss::Cell");
			cellsData->add(cellData);
			
			cellData->add(Lump::stringData(cell->name));
			cellData->add(cellMapData);
			cellData->add(Lump::s16Data((int)cellIndex));
			cellData->add(Lump::s16Data((int)cell->pos.x));
			cellData->add(Lump::s16Data((int)cell->pos.y));
			cellData->add(Lump::s16Data((int)cell->size.x));
			cellData->add(Lump::s16Data((int)cell->size.y));
			cellData->add(Lump::s16Data(0));	// reserved
		}
	}

	// パーツ、アニメ情報
	for (int packIndex = 0; packIndex < (int)proj->animeList.size(); packIndex++)
	{
		const SsAnimePack* animePack = proj->animeList[packIndex];
		const SsModel& model = animePack->Model;
		
		// AnimePackData
		Lump* animePackData = Lump::set("ss::AnimePackData");
		packDataArray->add(animePackData);

		Lump* partDataArray = Lump::set("ss::PartData[]", true);
		Lump* animeDataArray = Lump::set("ss::AnimationData[]", true);

		animePackData->add(Lump::stringData(animePack->name));
		//全角チェック
		if ( isZenkaku( &animePack->name ) == true )
		{
			std::cerr << "エラー：全角が使用されている: " << animePack->name << "\n";
			convert_error_exit = true;	//エラーが発生コンバート失敗
		}
		animePackData->add(partDataArray);
		animePackData->add(animeDataArray);
		animePackData->add(Lump::s16Data((int)model.partList.size()));
		animePackData->add(Lump::s16Data((int)animePack->animeList.size()));

		// パーツ情報（モデル）の出力
		for (int partIndex = 0; partIndex < (int)model.partList.size(); partIndex++)
		{
			const SsPart* part = model.partList[partIndex];

			// PartData
			Lump* partData = Lump::set("ss::PartData");
			partDataArray->add(partData);

			partData->add(Lump::stringData(part->name));
			//全角チェック
			if ( isZenkaku( &part->name ) == true )
			{
				std::cerr << "エラー：全角が使用されている: " << part->name << "\n";
				convert_error_exit = true;	//エラーが発生コンバート失敗
			}
			partData->add(Lump::s16Data(part->arrayIndex));
			partData->add(Lump::s16Data(part->parentIndex));
			partData->add(Lump::s16Data(part->type));
			partData->add(Lump::s16Data(part->boundsType));
			partData->add(Lump::s16Data(part->alphaBlendType));
			//インスタンスアニメ名
			if ( part->refAnime == "" )
			{
				const SsString str = "";
//				partData->add(Lump::s16Data((int)str.length()));				//文字列のサイズ
				partData->add(Lump::stringData(str));							//文字列
			}
			else
			{
				const SsString str = part->refAnimePack + "/" + part->refAnime;
//				partData->add(Lump::s16Data((int)str.length()));				//文字列のサイズ
				partData->add(Lump::stringData(str));							//文字列
			}


		}

		
		// アニメ情報の出力
		SsCellMapList* cellMapList = new SsCellMapList();	// SsAnimeDecoderのデストラクタで破棄される
		SsAnimeDecoder decoder;
//		const SsKeyframe* key;

		for (int animeIndex = 0; animeIndex < (int)animePack->animeList.size(); animeIndex++)
		{
			SsAnimePack* animePack = proj->getAnimePackList()[packIndex];
			SsModel* model = &animePack->Model;
			SsAnimation* anime = animePack->animeList[animeIndex];
			
			cellMapList->set(proj, animePack);
			decoder.setAnimation(model, anime, cellMapList, proj);
			std::list<SsPartState*>& partList = decoder.getPartSortList();
			
			// AnimationData
			Lump* animeData = Lump::set("ss::AnimationData");
			animeDataArray->add(animeData);
			

			// パーツごとのアニメーションパラメータ初期値
			// ※とりあえず先頭フレームの値を初期値にしているが、一番使われている値を初期値にすべきかも
			size_t numParts = model->partList.size();
			std::vector<PartInitialData> initialDataList;
			
			decoder.setPlayFrame(0);
			decoder.update();

			Lump* initialDataArray = Lump::set("ss::AnimationInitialData[]", true);
			int sortedOrder = 0;
			foreach(std::vector<SsPartAndAnime>, decoder.getPartAnime(), it)
			{
				SsPart* part = it->first;
//				SsPartAnime* partAnime = it->second;
				
				const SsPartState* state = findState(partList, part->arrayIndex);
				
				PartInitialData init;
				init.sortedOrder = sortedOrder++;
				init.index = state->index;

				int flags = 0;
				if (state->hide)  flags |= PART_FLAG_INVISIBLE;
				//イメージ反転を適用する
				bool hFlip = state->hFlip ^ state->imageFlipH;
				bool vFlip = state->vFlip ^ state->imageFlipV;
				if (hFlip) flags |= PART_FLAG_FLIP_H;
				if (vFlip) flags |= PART_FLAG_FLIP_V;

				init.flags = flags;

				// cellIndex
				int cellIndex = -1;
				if (state->cellValue.cell) cellIndex = (*cellList)[state->cellValue.cell];
				init.cellIndex = cellIndex;
				
//				// posx
//				key = findFirstKey(partAnime, SsAttributeKind::posx);
//				init.posX = key != NULL ? (int)key->value._float : 0;
//				// posy
//				key = findFirstKey(partAnime, SsAttributeKind::posy);
//				init.posY = key != NULL ? (int)key->value._float : 0;

				init.posX = (int)(state->position.x * DOT);
				init.posY = (int)(state->position.y * DOT);
				init.anchorX = state->pivotOffset.x + 0.5f;
				init.anchorY = state->pivotOffset.y + 0.5f;
				init.rotation = state->rotation.z;

				float size_scale_x = state->scale.x;
				float size_scale_y = state->scale.y;

				init.scaleX = size_scale_x;
				init.scaleY = size_scale_y;
				init.opacity = (int)(state->alpha * 255);
				//サイズはエディターでは初期値が1が設定されているが、
				//本来であればキーがないときはセルのサイズが初期値になる
				init.size_X = state->size.x;
				init.size_Y = state->size.y;
				SsCell * cell = state->cellValue.cell;
				if ( cell )
				{
					//セルデータがある場合はセルのサイズを初期値にする
					init.size_X = cell->size.x;
					init.size_Y = cell->size.y;
				}
				init.uv_move_X = state->uvTranslate.x;
				init.uv_move_Y = state->uvTranslate.y;
				init.uv_rotation = state->uvRotation;
				init.uv_scale_X = state->uvScale.x;
				init.uv_scale_Y = state->uvScale.y;




				initialDataList.push_back(init);
				
				Lump* initialData = Lump::set("ss::AnimationInitialData");
				initialDataArray->add(initialData);
				initialData->add(Lump::s16Data(init.index));
				initialData->add(Lump::s16Data(0)); //ダミーデータ
				initialData->add(Lump::s32Data(init.flags));
				initialData->add(Lump::s16Data(init.cellIndex));
				initialData->add(Lump::s16Data(init.posX));
				initialData->add(Lump::s16Data(init.posY));
				initialData->add(Lump::s16Data(init.opacity));
				initialData->add(Lump::floatData(init.anchorX));
				initialData->add(Lump::floatData(init.anchorY));
				initialData->add(Lump::floatData(init.rotation));
				initialData->add(Lump::floatData(init.scaleX));
				initialData->add(Lump::floatData(init.scaleY));
				initialData->add(Lump::floatData(init.size_X));
				initialData->add(Lump::floatData(init.size_Y));
				initialData->add(Lump::floatData(init.uv_move_X));
				initialData->add(Lump::floatData(init.uv_move_Y));
				initialData->add(Lump::floatData(init.uv_rotation));
				initialData->add(Lump::floatData(init.uv_scale_X));
				initialData->add(Lump::floatData(init.uv_scale_Y));
			}


			// フレーム毎データ
			Lump* frameDataIndexArray = Lump::set("ss::ss_u16*[]", true);
			for (int frame = 0; frame < decoder.getAnimeEndFrame(); frame++)
			{
				// パラメータを計算し更新する
				decoder.setPlayFrame(static_cast<float>(frame));
				decoder.update();
				
				// パーツの描画順が初期値と変わっていないかチェックする
				bool prioChanged = false;
//				int initialDataListIndex = 0;
//				foreach(std::list<SsPartState*>, partList, it)
//				{
//					const SsPartState* state = *it;
//					if (state->index != sortedInitialDataList.at(initialDataListIndex++).index)
//					{
//						// 描画順が変わっている
//						prioChanged = true;
//						break;
//					}
//				}
				
				
				// パーツごとのデータを出力する
				Lump* frameData = Lump::set("ss::ss_u16[]", true);
				frameDataIndexArray->add(frameData);
				
				Lump* frameFlag = Lump::s16Data(0);
//				frameData->add(frameFlag);

				int outPartsCount = 0;
				foreach(std::list<SsPartState*>, partList, it)
				{
					const SsPartState* state = *it;
					
					//セルに設定された原点補正を取得
					SsVector2 pivot;
					pivot.x = 0;
					pivot.y = 0;

					SsCell * cell = state->cellValue.cell;
					float cpx = 0;
					float cpy = 0;
					if (cell)
					{
						// セルに設定された原点オフセットを適用する
						// ※セルの原点は中央が0,0で＋が右上方向になっている
						cpx = cell->pivot.x;
						if (state->hFlip) cpx = -cpx;	// 水平フリップによって原点を入れ替える
						// 上が＋で入っているのでここで反転する。
						cpy = -cell->pivot.y;
						if (state->vFlip) cpy = -cpy;	// 垂直フリップによって原点を入れ替える
					}

					// 次に原点オフセットアニメの値を足す
					pivot.x = cpx + state->pivotOffset.x;
					pivot.y = cpy + -state->pivotOffset.y;


					int cellIndex = -1;
					if (state->cellValue.cell) cellIndex = (*cellList)[state->cellValue.cell];
					
					// フラグのみパラメータ
					int s_flags = 0;
					if (state->hide)	s_flags |= PART_FLAG_INVISIBLE;
					//イメージ反転を適用する
					bool hFlip = state->hFlip ^ state->imageFlipH;
					bool vFlip = state->vFlip ^ state->imageFlipV;
					if (hFlip)			s_flags |= PART_FLAG_FLIP_H;
					if (vFlip)			s_flags |= PART_FLAG_FLIP_V;
					
					// 以下、規定値のときは出力を省略する
					int p_flags = 0;
					const PartInitialData& init = initialDataList.at(state->index);
					if (cellIndex != init.cellIndex)                 p_flags |= PART_FLAG_CELL_INDEX;
					if ((int)( state->position.x * DOT ) != init.posX)     p_flags |= PART_FLAG_POSITION_X;
					if ((int)( state->position.y * DOT ) != init.posY)     p_flags |= PART_FLAG_POSITION_Y;
					if (pivot.x + 0.5f != init.anchorX)              p_flags |= PART_FLAG_ANCHOR_X;
					if (pivot.y + 0.5f != init.anchorY)              p_flags |= PART_FLAG_ANCHOR_Y;
					if (state->rotation.z != init.rotation)          p_flags |= PART_FLAG_ROTATION;

					float size_scale_x = state->scale.x;
					float size_scale_y = state->scale.y;
					if (size_scale_x != init.scaleX)               p_flags |= PART_FLAG_SCALE_X;
					if (size_scale_y != init.scaleY)               p_flags |= PART_FLAG_SCALE_Y;
					if ((int)state->alpha * 255 != init.opacity)   p_flags |= PART_FLAG_OPACITY;
					if (state->size.x != init.size_X)              p_flags |= PART_FLAG_SIZE_X;
					if (state->size.y != init.size_X)              p_flags |= PART_FLAG_SIZE_Y;
					if (state->uvTranslate.x != init.uv_move_X )   p_flags |= PART_FLAG_U_MOVE;
					if (state->uvTranslate.y != init.uv_move_Y)    p_flags |= PART_FLAG_V_MOVE;
					if (state->uvRotation != init.uv_rotation)     p_flags |= PART_FLAG_UV_ROTATION;
					if (state->uvScale.x != init.uv_scale_X)       p_flags |= PART_FLAG_U_SCALE;
					if (state->uvScale.y != init.uv_scale_Y)       p_flags |= PART_FLAG_V_SCALE;


					// カラーブレンド値を格納する必要があるかチェック
					int cb_flags = 0;
					if (state->is_color_blend)
					{
						switch (state->colorValue.target)
						{
							case SsColorBlendTarget::whole:
								if ( 
									  ( state->colorValue.color.rgba.a == 0 )
								   && ( state->colorValue.color.rgba.r == 0 )	
								   && ( state->colorValue.color.rgba.g == 0 )
								   && ( state->colorValue.color.rgba.b == 0 )	
								   )
								{
									//右のキーが単色、左のキーが4頂点などの場合に単色の色出力ができないため
									//フラグがあるのに単色の色が設定されていない場合は4頂点カラーとして出力
									cb_flags = VERTEX_FLAG_LT|VERTEX_FLAG_RT|VERTEX_FLAG_LB|VERTEX_FLAG_RB;
								}
								else
								{
									cb_flags = VERTEX_FLAG_ONE;
								}

								break;
							case SsColorBlendTarget::vertex:
								cb_flags = VERTEX_FLAG_LT|VERTEX_FLAG_RT|VERTEX_FLAG_LB|VERTEX_FLAG_RB;
								break;
							default:
								// Unsupported
								assert(false);
								break;
						}
						
						if (cb_flags) p_flags |= PART_FLAG_COLOR_BLEND;
					}

					// 頂点変形のオフセット値を格納する必要あるかチェック
					int vt_flags = 0;
					if (state->is_vertex_transform)
					{
						for (int vtxNo = 0; vtxNo < 4; vtxNo++)
						{
							// 小数点以下切り捨てで
							if ((int)state->vertexValue.offsets[vtxNo].x != 0
							 || (int)state->vertexValue.offsets[vtxNo].y != 0)
							{
								vt_flags |= 1 << vtxNo;
							}
						}

						if (vt_flags) p_flags |= PART_FLAG_VERTEX_TRANSFORM;
					}

					//インスタンス情報出力チェック
					if ( state->refAnime )
					{
						//とりあえず毎フレーム出力するが、更新があったときだけ出力しても済むかも。
						//その場合は処理をプレイヤーと合わせる必要あり。
						p_flags |= PART_FLAG_INSTANCE_KEYFRAME;
						p_flags |= PART_FLAG_INSTANCE_START;
						p_flags |= PART_FLAG_INSTANCE_END;
						p_flags |= PART_FLAG_INSTANCE_SPEED;
						p_flags |= PART_FLAG_INSTANCE_LOOP;
						p_flags |= PART_FLAG_INSTANCE_LOOP_FLG;
					}

					

//					if (!prioChanged && s_flags == init.flags && p_flags == 0)
//					{
//						// このパーツは出力を省略する
//						continue;
//					}
					
					// パーツの座標値、回転、スケールなどを出力する
					outPartsCount++;
					frameData->add(Lump::s16Data(state->index));
//					frameData->add(Lump::s16Data(0));				//32bitアライメント用ダミーデータ
					frameData->add(Lump::s32Data(s_flags | p_flags));

					if (p_flags & PART_FLAG_CELL_INDEX) frameData->add(Lump::s16Data(cellIndex));
					if (p_flags & PART_FLAG_POSITION_X) frameData->add(Lump::s16Data((int)(state->position.x * DOT)));
					if (p_flags & PART_FLAG_POSITION_Y) frameData->add(Lump::s16Data((int)(state->position.y * DOT)));

					if (p_flags & PART_FLAG_ANCHOR_X) frameData->add(Lump::floatData(pivot.x + 0.5f));
					if (p_flags & PART_FLAG_ANCHOR_Y) frameData->add(Lump::floatData(pivot.y + 0.5f));
					if (p_flags & PART_FLAG_ROTATION) frameData->add(Lump::floatData(state->rotation.z));	// degree
					if (p_flags & PART_FLAG_SCALE_X) frameData->add(Lump::floatData(size_scale_x));
					if (p_flags & PART_FLAG_SCALE_Y) frameData->add(Lump::floatData(size_scale_y));
					if (p_flags & PART_FLAG_OPACITY) frameData->add(Lump::s16Data((int)(state->alpha * 255)));

					if (p_flags & PART_FLAG_SIZE_X) frameData->add(Lump::floatData(state->size.x));
					if (p_flags & PART_FLAG_SIZE_Y) frameData->add(Lump::floatData(state->size.y));

					if (p_flags & PART_FLAG_U_MOVE) frameData->add(Lump::floatData(state->uvTranslate.x));
					if (p_flags & PART_FLAG_V_MOVE) frameData->add(Lump::floatData(state->uvTranslate.y));
					if (p_flags & PART_FLAG_UV_ROTATION) frameData->add(Lump::floatData(state->uvRotation));
					if (p_flags & PART_FLAG_U_SCALE) frameData->add(Lump::floatData(state->uvScale.x));
					if (p_flags & PART_FLAG_V_SCALE) frameData->add(Lump::floatData(state->uvScale.y));


					// 頂点変形データ
					if (p_flags & PART_FLAG_VERTEX_TRANSFORM)
					{
						// どの頂点のオフセット値が格納されているかのフラグ
						frameData->add(Lump::s16Data(vt_flags));

						// 各頂点のオフセット値
						for (int vtxNo = 0; vtxNo < 4; vtxNo++)
						{
							if (vt_flags & (1 << vtxNo))
							{
								frameData->add(Lump::s16Data((int)state->vertexValue.offsets[vtxNo].x));
								frameData->add(Lump::s16Data((int)state->vertexValue.offsets[vtxNo].y));
							}
						}
					}

					// カラーブレンドデータ
					if (p_flags & PART_FLAG_COLOR_BLEND)
					{
						// ブレンド方法と、単色もしくはどの頂点に対するカラー値が格納されているかをu16にまとめる
						int typeAndFlags = (int)state->colorValue.blendType | (cb_flags << 8);
						frameData->add(Lump::s16Data(typeAndFlags));

						if (cb_flags & VERTEX_FLAG_ONE)
						{
							frameData->add(Lump::floatData(state->colorValue.color.rate));
							frameData->add(Lump::colorData(state->colorValue.color.rgba.toARGB()));
						}
						else
						{
							for (int vtxNo = 0; vtxNo < 4; vtxNo++)
							{
								if (cb_flags & (1 << vtxNo))
								{
									frameData->add(Lump::floatData(state->colorValue.colors[vtxNo].rate));
									frameData->add(Lump::colorData(state->colorValue.colors[vtxNo].rgba.toARGB()));
								}
							}
						}
					}
					//インスタンス情報出力
					if ( p_flags & PART_FLAG_INSTANCE_KEYFRAME )
					{
						//ラベル位置にオフセットを加えた結果の開始フレーム
						frameData->add(Lump::s16Data(state->instanceValue.curKeyframe));
					}
					if ( p_flags & PART_FLAG_INSTANCE_START )
					{
						//ラベル位置にオフセットを加えた結果の開始フレーム
						frameData->add(Lump::s16Data(state->instanceValue.startFrame));
					}
					if ( p_flags & PART_FLAG_INSTANCE_END )
					{
						//ラベル位置にオフセットを加えた結果の終了フレーム
						frameData->add(Lump::s16Data(state->instanceValue.endFrame));
					}
					if ( p_flags & PART_FLAG_INSTANCE_SPEED )
					{
						frameData->add(Lump::floatData(state->instanceValue.speed));
					}
					if ( p_flags & PART_FLAG_INSTANCE_LOOP )
					{
						frameData->add(Lump::s16Data(state->instanceValue.loopNum));
					}
					if ( p_flags & PART_FLAG_INSTANCE_LOOP_FLG )
					{
						int iflags = 0;
						if ( state->instanceValue.infinity )
						{
							iflags = iflags | INSTANCE_LOOP_FLAG_INFINITY;
						}
						if ( state->instanceValue.reverse )
						{
							iflags = iflags | INSTANCE_LOOP_FLAG_REVERSE;
						}
						if ( state->instanceValue.pingpong )
						{
							iflags = iflags | INSTANCE_LOOP_FLAG_PINGPONG;
						}
						if ( state->instanceValue.independent )
						{
							iflags = iflags | INSTANCE_LOOP_FLAG_INDEPENDENT;
						}
						frameData->add(Lump::s16Data(iflags));
					}
				}
				
				// 出力されたパーツ数と、描画順の変更があるかのフラグ
				frameFlag->data.i = outPartsCount | (prioChanged ? 0x8000 : 0);
			}
			
			
			
			// ユーザーデータ
			Lump* userDataIndexArray = Lump::set("ss::ss_u16*[]", true);
			bool hasUserData = false;

			for (int frame = 0; frame < decoder.getAnimeEndFrame(); frame++)
			{
				Lump* userData = Lump::set("ss::ss_u16[]", true);
				int partsCount = 0;

				foreach(std::vector<SsPartAndAnime>, decoder.getPartAnime(), it)
				{
					SsPart* part = it->first;
					SsPartAnime* partAnime = it->second;
					if (!partAnime) continue;
					
					foreach(SsAttributeList, partAnime->attributes, attrIt)
					{
						SsAttribute* attr = *attrIt;
						if (attr->tag != SsAttributeKind::user) continue;
						
						// このフレームのデータを含む?
						if (attr->key_dic.find(frame) != attr->key_dic.end())
						{
							hasUserData = true;
							partsCount++;
							
							const SsKeyframe* keyframe = attr->key_dic.at(frame);
							SsUserDataAnime udat;
							GetSsUserDataAnime(keyframe, udat);

							int flags = 0;
							if (udat.useInteger) flags |= USER_DATA_FLAG_INTEGER;
							if (udat.useRect)    flags |= USER_DATA_FLAG_RECT;
							if (udat.usePoint)   flags |= USER_DATA_FLAG_POINT;
							if (udat.useString)  flags |= USER_DATA_FLAG_STRING;

							userData->add(Lump::s16Data(flags));
							userData->add(Lump::s16Data(part->arrayIndex));

							if (udat.useInteger)
							{
								userData->add(Lump::s32Data(udat.integer));
							}
							if (udat.useRect)
							{
								userData->add(Lump::s32Data(udat.rect.x));
								userData->add(Lump::s32Data(udat.rect.y));
								userData->add(Lump::s32Data(udat.rect.w));
								userData->add(Lump::s32Data(udat.rect.h));
							}
							if (udat.usePoint)
							{
								userData->add(Lump::s32Data((int)udat.point.x));
								userData->add(Lump::s32Data((int)udat.point.y));
							}
							if (udat.useString)
							{
								const SsString& str = udat.string;
								userData->add(Lump::s16Data((int)str.length()));
								userData->add(Lump::stringData(str));
							}
						}
					}
				}
				
				if (partsCount)
				{
					userData->addFirst(Lump::s16Data(partsCount));
					userDataIndexArray->add(userData);
				}
				else
				{
					userDataIndexArray->add(Lump::s32Data(0));
					delete userData;
				}
			}

			// ラベルデータ
			Lump* LabelDataIndexArray = Lump::set("ss::ss_u16*[]", true);
			bool hasLabelData = false;
			int label_idx = 0;
			for (label_idx = 0; label_idx < (int)anime->labels.size(); label_idx++)
			{
				Lump* labelData = Lump::set("ss::ss_u16[]", true);

				SsString str;
				str = anime->labels[label_idx]->name;
				//全角チェック
				if ( isZenkaku( &str ) == true )
				{
					std::cerr << "エラー：全角が使用されている: " << str << "\n";
					convert_error_exit = true;	//エラーが発生コンバート失敗
				}

//				labelData->add(Lump::s16Data((int)str.length()));				//文字列のサイズ
				labelData->add(Lump::stringData(str));							//文字列
				labelData->add(Lump::s16Data(anime->labels[label_idx]->time));	//設定されたフレーム
				hasLabelData = true;

				LabelDataIndexArray->add(labelData);

			}
			if ( hasLabelData == false )
			{
				LabelDataIndexArray->add(Lump::s32Data(0));
			}
			
			animeData->add(Lump::stringData(anime->name));
			animeData->add(initialDataArray);
			animeData->add(frameDataIndexArray);
			animeData->add(hasUserData ? userDataIndexArray : Lump::s32Data(0));
			animeData->add(hasLabelData ? LabelDataIndexArray : Lump::s32Data(0));
			animeData->add(Lump::s16Data(decoder.getAnimeEndFrame()));
			animeData->add(Lump::s16Data(anime->settings.fps));
			animeData->add(Lump::s16Data(label_idx));				//ラベルデータ数
		}
		
	}
	
	return topLump;
}



void convertProject(const std::string& outPath, LumpExporter::StringEncoding encoding, const std::string& sspjPath, const std::string& imageBaseDir, const std::string& creatorComment)
{
	SSTextureFactory texFactory(new SSTextureBMP());
	SsProject* proj = ssloader_sspj::Load(sspjPath);
	Lump* lump = parseParts(proj, imageBaseDir);

	if ( convert_error_exit == true )
	{
		//データにエラーがありコンバートを中止した
		//ファイルの出力を行なわない
	}
	else
	{

		std::fstream out;
		out.open(outPath.c_str(), std::ios_base::binary | std::ios_base::out);
		LumpExporter::saveBinary(out, encoding, lump, creatorComment);

	/////////////
	#if 0
		out.close();
		std::string hOutPath = outPath + ".h";
		out.open(hOutPath.c_str(), std::ios_base::binary | std::ios_base::out);
		LumpExporter::saveCSource(out, encoding, lump, "animeData", creatorComment);
	#endif
	/////////////

	//	delete lump;
		delete proj;
	}
}








#define APP_NAME		"Ss5Converter"
#define APP_VERSION		"1.0.0 (Build: " __DATE__ " " __TIME__ ")"


static const char* HELP =
"\n"
APP_NAME " converter version " APP_VERSION "\n"
"usage: " APP_NAME " Input files(.sspj) ...\n"
"\n"
"option:\n"
"  -h      Display usage.\n"
"  -v      Verbose mode.\n"
"  -e arg  Encoding of output file (UTF8/SJIS) default:UTF8\n"
"  -p arg  Specify image file load base path.\n"
"\n";


struct Options
{
	typedef std::vector<std::string> StringList;

	bool							isHelp;
	bool							isVerbose;
	StringList						inList;
	LumpExporter::StringEncoding	encoding;
	std::string						imageBaseDir;


	Options()
	: isHelp(false)
	, isVerbose(false)
	, encoding(LumpExporter::UTF8)
	{}
};




class ArgumentPointer
{
	int _argc;
	const char** _argv;
	int _index;

public:
	ArgumentPointer(int argc, const char* argv[])
	: _argc(argc), _argv(argv), _index(1)
	{}

	bool hasNext() const
	{
		return _index < _argc;
	}
	
	std::string next()
	{
		assert(hasNext());
		return std::string(_argv[_index++]);
	}
};


bool parseOption(Options& options, const std::string& opt, ArgumentPointer& args, std::string& illegalArgument)
{
	if (opt == "-h")
	{
		options.isHelp = true;
	}
	else if (opt == "-v")
	{
		options.isVerbose = true;
	}
	else if (opt == "-e")
	{
		if (!args.hasNext()) return false;

		// 大文字に変換してからチェックする
		std::string enc = args.next();
		std::string uppered = enc;
		std::transform(uppered.begin(), uppered.end(), uppered.begin(), toupper);
		
		if      (uppered == "UTF8") options.encoding = LumpExporter::UTF8;
		else if (uppered == "SJIS") options.encoding = LumpExporter::SJIS;
		else
		{
			// not support encoding
			illegalArgument = opt + " " + enc;
			return false;
		}
	}
	else if (opt == "-p")
	{
		if (!args.hasNext()) return false;

		options.imageBaseDir = args.next();
	}
	else
	{
		// unknown
		illegalArgument = opt;
		return false;
	}

	// success
	return true;
}


bool parseArguments(Options& options, int argc, const char* argv[], std::string& illegalArgument)
{
	Options::StringList inList;

	ArgumentPointer args(argc, argv);
	if (!args.hasNext())
	{
		options.isHelp = true;
		return true;
	}

	while (args.hasNext())
	{
		std::string name = args.next();
		if (name[0] == '-')
		{
			bool success = parseOption(options, name, args, illegalArgument);
			if (!success)
			{
				return false;
			}
		}
		else
		{
			inList.push_back(name);
		}
	}

	options.inList = inList;
	// success
	return true;
}






int convertMain(int argc, const char * argv[])
{
	// 引数をパースする
	Options options;
	std::string illegalArgument;
	bool success = parseArguments(options, argc, argv, illegalArgument);
	if (!success)
	{
		// 引数が不正
		std::cerr << "Invalid arguments: " << illegalArgument << std::endl;
		std::cout << HELP;
		return SSPC_ILLEGAL_ARGUMENT;
	}

	if (options.isHelp)
	{
        // usage表示。正常終了にしています。
		std::cout << HELP;
		return SSPC_SUCCESS;
	}
	
	
	// *** 入力ファイル名チェック
	std::vector<std::string> sources;
	{
		Options::StringList& in = options.inList;
	
		bool error = false;
		for (Options::StringList::iterator it = in.begin(); it != in.end(); it++)
		{
			const std::string& str = *it;
		
#ifdef _WIN32
			// Win32プラットフォーム用コード。Win32APIを使ってワイルドカード展開する
			std::vector<std::string> fileList = FileUtil::findPath(str);
			if (!fileList.empty())
			{
				std::copy(fileList.begin(), fileList.end(), std::back_inserter(sources));
			}
			else
			{
				std::cerr << "Cannot find input file: " << str << std::endl;
				error = true;
			}
#else
			// Mac/Unixプラットフォーム用コード
			// 本当にファイルが存在するか確認し、見つからないものがあるときはエラーとする
			struct stat st;
			int result = stat(str.c_str(), &st);
			if (result == 0)
			{
				sources.push_back(str);
			}
			else
			{
				std::cerr << "Cannot find input file: " << str << std::endl;
				error = true;
			}
#endif
		}
		if (error)
        {
            return SSPC_NOT_EXIST_INPUT_FILE;
        }
	}
	
	
	
	std::string creatorComment = "Created by " APP_NAME " v" APP_VERSION;
	LumpExporter::StringEncoding encoding = options.encoding;
	
	// コンバート実行
	for (std::vector<std::string>::const_iterator it = sources.begin(); it != sources.end(); it++)
	{
		std::string sspjPath = *it;
		std::string outPath = FileUtil::replaceExtension(sspjPath, ".sspj", ".ssbp");
		
		if (options.isVerbose)
		{
			std::cout << "Convert: " << sspjPath << " -> " << outPath << std::endl;
		}
		
		convertProject(outPath, encoding, sspjPath, options.imageBaseDir, creatorComment);
	}

	if ( convert_error_exit == true )
	{
		//データにエラーがありコンバートを中止した
	    return SSPC_SSPJ_PARSE_FAILED;
	}
	else
	{
	    return SSPC_SUCCESS;
	}
}




int main(int argc, const char * argv[])
{
	int resultCode = convertMain(argc, argv);
	return resultCode;
}


