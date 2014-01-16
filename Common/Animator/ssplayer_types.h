#ifndef __SSPLAYER_TYPES__
#define __SSPLAYER_TYPES__

#include "../loader/ssloader.h"
#include "../Helper/ssHelper.h"



///頂点変形キーの４頂点変形値
struct SsVertexAnime
{
	SsPoint2	offsets[4];	///< 各頂点の移動オフセット値
};



///カラーブレンド使用時のブレンドタイプとカラー値
struct SsColorAnime
{
	SsColorBlendTarget::_enum	target;		//ブレンドの適用方法( 単色。全体 , 頂点単位 )
	SsBlendType::_enum			blendType;	//ブレンド種別 (mix　乗算　加算　減算）
	SsColorBlendValue			color;		//単色。全体の場合に使用されるカラー値
	SsColorBlendValue			colors[4];	//頂点単位の場合使用されるカラー値
};





#endif
