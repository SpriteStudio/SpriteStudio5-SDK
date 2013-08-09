#ifndef __SSPLAYER_RENDER__
#define __SSPLAYER_RENDER__

class SsPartState;

class SsRender
{
private:
	static bool	m_isInit;

public:
	SsRender(){}
	virtual ~SsRender(){}

	static void	shaderInit();
	static void	RenderPart( SsPartState* state );
};


#endif
