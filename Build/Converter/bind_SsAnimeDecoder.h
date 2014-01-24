#ifndef __bind_ANIMESTATE__
#define __bind_ANIMESTATE__


#include "package_SpriteStudio.h"


class Bind_SsPartAnime;
class Bind_SsPart;
class SsAnimeDecoder;
class Bind_SsAnimeDecoder
{
private:

public:
	SsAnimeDecoder*	m_decoder;

	Bind_SsAnimeDecoder();
	virtual ~Bind_SsAnimeDecoder();
	bool debug();

	void setFrame( int f );
	void update();

	Bind_SsPart*		getPart(int index);
	int					getPartNum();
	Bind_SsPartAnime*	getPartAnime(int index);
};



#endif
