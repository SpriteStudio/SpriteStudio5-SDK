#ifndef __bind_ANIMEPACK__
#define __bind_ANIMEPACK__


#include "package_SpriteStudio.h"



class Bind_SsAnimeDecoder;
class Bind_SsAnimePack
{
public:
	SsAnimePack*	m_animepack;

	Bind_SsAnimePack() : m_animepack(0){}
	bool debug();

	Bind_SsAnimeDecoder*	getAnimeDecoderByName( const char* animename );
	Bind_SsAnimeDecoder*	getAnimeDecoderByIndex( int index );
	
};



#endif
