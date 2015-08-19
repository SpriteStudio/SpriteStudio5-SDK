#ifndef __SSPLAYER_RENDER_DX9__
#define __SSPLAYER_RENDER_DX9__

#include "ssplayer_render.h"

struct SsPartState;

class SsRenderDX9 : public ISsRenderer
{
private:
	//static bool	m_isInit;

public:
	SsRenderDX9(){}
	virtual ~SsRenderDX9(){}

	virtual void	initialize();
	virtual void	renderSetup();
	virtual void	renderPart( SsPartState* state );
	virtual void	renderSpriteSimple( float matrix[16], 
										int width, int height, 
										SsVector2& pivot , 
										SsVector2 uv1, SsVector2 uv2, 
										const SsFColor& color ){}

	virtual void	SetAlphaBlendMode(SsBlendType::_enum type){}


};

#endif
