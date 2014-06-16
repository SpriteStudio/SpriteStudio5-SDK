//
#include "bind_SsAnimeDecoder.h"


#include "bind_sspartanime.h"


Bind_SsAttribute*	Bind_SsPartAnime::getAttribute(int Kind )
{ 


	size_t num = this->bind_inst->attributes.size();

	for ( size_t n = 0 ; n < num ; n++ )
	{
		SsAttribute* a = this->bind_inst->attributes[n];
		if ( a->tag == Kind ) return new Bind_SsAttribute(a);

	}

	return new Bind_SsAttribute(0); 
}
