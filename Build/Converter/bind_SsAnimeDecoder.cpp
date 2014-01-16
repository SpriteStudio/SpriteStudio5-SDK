//
#include "bind_SsAnimeDecoder.h"


#include "ssplayer_animedecode.h"




Bind_SsAnimeDecoder::Bind_SsAnimeDecoder()
{
	puts("construct Bind_SsAnimeState");	

	m_decoder = new SsAnimeDecoder();

}

