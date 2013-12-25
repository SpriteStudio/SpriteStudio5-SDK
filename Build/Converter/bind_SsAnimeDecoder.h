#ifndef __bind_ANIMESTATE__
#define __bind_ANIMESTATE__


#include "package_SpriteStudio.h"



class Bind_SsAnimeDecoder
{
public:
	Bind_SsAnimeDecoder()
	{
		puts("construct Bind_SsAnimeState");	
	}
	virtual ~Bind_SsAnimeDecoder()
	{
		puts( "destruct Bind_SsAnimeState" );
	}
	bool debug()
	{
		puts( "debug Bind_SsAnimeState" );
		return true;
	}
};



#endif
