#ifndef __bind_ANIMESTATE__
#define __bind_ANIMESTATE__


#include "package_SpriteStudio.h"


class SsAnimeDecoder;
class Bind_SsAnimeDecoder
{
private:

public:
	SsAnimeDecoder*	m_decoder;

	Bind_SsAnimeDecoder();
	virtual ~Bind_SsAnimeDecoder();
	bool debug();

};



#endif
