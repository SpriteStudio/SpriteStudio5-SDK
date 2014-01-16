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

}

void	SsRenderDX9::renderSetup()
{

}


void	SsRenderDX9::renderPart( SsPartState* state )
{


}
