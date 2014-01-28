#ifndef __bind_SSPARTANIME__
#define __bind_SSPARTANIME__


#include "package_SpriteStudio.h"


class Bind_SsAttribute;
class Bind_SsPartAnime : public myPyBinder<SsPartAnime>
{
private:
public:
		
	Bind_SsPartAnime(){}
	Bind_SsPartAnime(SsPartAnime* panime){ bind(panime);}
	virtual ~	Bind_SsPartAnime(){}

	const char* name(){ return bind_inst->partName.c_str(); }
	
	Bind_SsAttribute*	getAttribute(int Kind );

};



#endif
