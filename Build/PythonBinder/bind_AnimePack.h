#ifndef __bind_ANIMEPACK__
#define __bind_ANIMEPACK__


#include "package_SpriteStudio.h"



class Bind_SsAnimeDecoder;
class SsCellMapList;

class Bind_SsAnimePack
{
public:
	SsAnimePack*		m_animepack;
	SsCellMapList*		m_cellmap;
	SsProject*			m_proj;

	Bind_SsAnimePack();
	bool debug();

	Bind_SsAnimeDecoder*	getAnimeDecoderByName( const char* animename );
	Bind_SsAnimeDecoder*	getAnimeDecoderByIndex( int index );
	
};



#endif
