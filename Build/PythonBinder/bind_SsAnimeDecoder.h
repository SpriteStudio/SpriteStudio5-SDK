#ifndef __bind_ANIMESTATE__
#define __bind_ANIMESTATE__


#include "package_SpriteStudio.h"


class Bind_SsPartAnime;
class Bind_SsPart;
class SsAnimeDecoder;
class Bind_SsPartState;

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

	int					getPartNum();
	Bind_SsPart*		getPart(int index);
	Bind_SsPartAnime*	getPartAnime(int index);
	Bind_SsPartState*	getPartState( int index );

	int	getFrameLength();
	int	getAnimeFPS();


};



#endif
