#ifndef __SSPLAYER_RENDER_GL__
#define __SSPLAYER_RENDER_GL__

#include "../Animator/ssplayer_render.h"

struct SsPartState;

class SsRenderGL : public ISsRenderer
{
private:
	//static bool	m_isInit;

public:
	SsRenderGL(){}
	virtual ~SsRenderGL(){}

	virtual void	initialize();
	virtual void	renderSetup();
	virtual void	renderPart( SsPartState* state );

	virtual void	renderSpriteSimple( float matrix[16], 
										int width, int height, 
										SsVector2& pivot , 
										SsVector2 uv1, SsVector2 uv2, 
										const SsFColor& color );

	virtual void	SetAlphaBlendMode(SsBlendType::_enum type);
	virtual void	SetTexture( SsCellValue* cell );



};

#endif
