#ifndef __SSPLAYER_RENDER_GL__
#define __SSPLAYER_RENDER_GL__

#include "ssplayer_render.h"

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
};

#endif
