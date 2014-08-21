//
//  SS5PlayerData.h
//

#ifndef SS5PlayerData_h
#define SS5PlayerData_h

namespace ss
{

typedef short			ss_s16;
typedef unsigned short	ss_u16;
typedef unsigned int	ss_u32;
typedef int				ss_s32;
typedef int				ss_offset;


enum BlendType
{
	BLEND_MIX,		///< 0 ブレンド（ミックス）
	BLEND_MUL,		///< 1 乗算
	BLEND_ADD,		///< 2 加算
	BLEND_SUB		///< 3 減算
};


struct CellMap
{
	ss_offset	name;			// const char*
	ss_offset	imagePath;		// const char*
	ss_s16		index;
	ss_s16		reserved;
};


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


struct AnimationData
{
	ss_offset	name;			// const char*
	ss_offset	defaultData;	// const AnimationInitialData*
	ss_offset	frameData;		// const ss_s16*
	ss_offset	userData;		// const ss_s16*
	ss_s16		numFrames;
	ss_s16		fps;
};


struct AnimationInitialData
{
	ss_s16		index;
	ss_s16		dummy;
	ss_u32		flags;
	ss_s16		cellIndex;
	ss_s16		positionX;
	ss_s16		positionY;
	ss_u16		opacity;
	float		anchorX;
	float		anchorY;
	float		rotation;
	float		scaleX;
	float		scaleY;
	float		uv_move_X;
	float		uv_move_Y;
	float		uv_rotation;
	float		uv_scale_X;
	float		uv_scale_Y;
};


struct PartData
{
	ss_offset	name;			// const char*
	ss_s16		index;
	ss_s16		parentIndex;
	ss_s16		type;
	ss_s16		boundsType;
	ss_s16		alphaBlendType;	// BlendType
};


struct AnimePackData
{
	ss_offset	name;			// const char*
	ss_offset	parts;			// const PartData*
	ss_offset	animations;		// const AnimationData*
	ss_s16		numParts;
	ss_s16		numAnimations;
};


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
