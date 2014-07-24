#ifndef __SSPLAYER_RENDER__
#define __SSPLAYER_RENDER__

struct SsPartState;


class ISsRenderer
{
public:
	ISsRenderer(){}
	virtual ~ISsRenderer(){}
public:
	virtual void	initialize() = 0;
	virtual void	renderSetup() = 0;
	virtual void	renderPart(SsPartState* state) = 0;
};

class	SsCurrentRenderer
{
private:
	static ISsRenderer*	m_currentrender;
public:
	SsCurrentRenderer(){}
	SsCurrentRenderer(ISsRenderer* render)
	{
		SetCurrentRender(render);
	}
	virtual ~SsCurrentRenderer()
	{
		if ( m_currentrender ) delete m_currentrender;
	}
	static void	SetCurrentRender( ISsRenderer* render )
	{ 
		render->initialize();
		m_currentrender = render; 
	}
	static ISsRenderer*	getRender(){ return m_currentrender; } 
};



#endif
