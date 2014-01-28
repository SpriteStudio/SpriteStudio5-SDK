#include "ssplayer_animedecode.h"
#include "bind_SsKeyFrame.h"


SsColorAnime&	Bind_SsKeyframe::getColorAnime()
{
	
	GetSsColorValue( this->bind_inst , m_tempcolor );
	
	return m_tempcolor;
}


SsVertexAnime&	Bind_SsKeyframe::getVertexAnime()
{
	
	GetSsVertexAnime( this->bind_inst , m_tempvertex );

	return m_tempvertex;

}

