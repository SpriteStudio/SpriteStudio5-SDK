/**
*  SS5PlayerData.h
*/

#ifndef SS5PlayerData_h
#define SS5PlayerData_h

namespace ss
{

/*
* ssbpに含まれるデータの定義
*/

/**
* 型定義
*/
typedef short			ss_s16;
typedef unsigned short	ss_u16;
typedef unsigned int	ss_u32;
typedef int				ss_s32;
typedef int				ss_offset;

/**
* セルマップ
*/
struct CellMap
{
	ss_offset	name;			// const char*
	ss_offset	imagePath;		// const char*
	ss_s16		index;
	ss_s16		reserved;
};


/**
* セル
*/
struct Cell
{
	ss_offset	name;			// const char*
	ss_offset	cellMap;		// const CellMap*
	ss_s16		indexInCellMap;
	ss_s16		x;
	ss_s16		y;
	ss_u16		width;
	ss_u16		height;
	ss_s16		reserved;
};


/**
* アニメ情報
*/
struct AnimationData
{
	ss_offset	name;			// const char*
	ss_offset	defaultData;	// const AnimationInitialData*
	ss_offset	frameData;		// const ss_s16*
	ss_offset	userData;		// const ss_s16*
	ss_offset	labelData;		// const ss_s16*
	ss_s16		numFrames;
	ss_s16		fps;
	ss_s16		labelNum;
};


/**
* 各パーツの初期値
*/
struct AnimationInitialData
{
	ss_s16		index;
	ss_s16		dummy;
	ss_u32		flags;
	ss_s16		cellIndex;
	ss_s16		positionX;
	ss_s16		positionY;
	ss_s16		positionZ;
	ss_u16		opacity;
	float		anchorX;
	float		anchorY;
	float		rotationX;
	float		rotationY;
	float		rotationZ;
	float		scaleX;
	float		scaleY;
	float		size_X;
	float		size_Y;
	float		uv_move_X;
	float		uv_move_Y;
	float		uv_rotation;
	float		uv_scale_X;
	float		uv_scale_Y;
	float		boundingRadius;
};

/**
* パーツ
*/
struct PartData
{
	ss_offset	name;			/// const char*
	ss_s16		index;			/// SS内のパーツインデックス
	ss_s16		parentIndex;	/// 親のパーツインデックス
	ss_s16		type;			/// パーツ種別
	ss_s16		boundsType;		/// 当たり判定種類
	ss_s16		alphaBlendType;	/// BlendType
	ss_offset	refname;		/// const char*　インスタンスとして配置されるアニメーション名
};

/**
* アニメーションパック情報
*/
struct AnimePackData
{
	ss_offset	name;			// const char*
	ss_offset	parts;			// const PartData*
	ss_offset	animations;		// const AnimationData*
	ss_s16		numParts;
	ss_s16		numAnimations;
};

/**
* プロジェクト情報
*/
struct ProjectData
{
	ss_u32		dataId;
	ss_u32		version;
	ss_u32		flags;
	ss_offset	imageBaseDir;	// const char*
	ss_offset	cells;			// const Cell*
	ss_offset	animePacks;		// const AnimePackData*
	ss_s16		numCells;
	ss_s16		numAnimePacks;
};

};	// namespace ss

#endif
