#ifndef __BIND_SSKYEFRAME__
#define __BIND_SSKYEFRAME__


#include "package_SpriteStudio.h"

class Bind_SsKeyframe : public myPyBinder<SsKeyframe>
{
public:
	Bind_SsKeyframe(){}
	Bind_SsKeyframe(SsKeyframe* a){bind(a);}
	virtual ~Bind_SsKeyframe(){}

	int	time(){ return bind_inst->time; }
};


#endif
