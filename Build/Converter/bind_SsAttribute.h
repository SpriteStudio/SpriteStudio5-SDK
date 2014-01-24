#ifndef __BIND_SSATTRIBUTE__
#define __BIND_SSATTRIBUTE__


#include "package_SpriteStudio.h"


class Bind_SsKeyframe;

class Bind_SsAttribute : public myPyBinder<SsAttribute>
{
private:
	SsKeyframe*	prevKey;
public:
	Bind_SsAttribute() : prevKey(0) {}
	Bind_SsAttribute(SsAttribute* a) { bind(a);}
	virtual ~Bind_SsAttribute()
	{
//		puts("destruct Bind_SsAttribute");
	}

	Bind_SsKeyframe*	firstKey();
	Bind_SsKeyframe*	nextKey();

	Bind_SsKeyframe*	findRightKey(int time);
	Bind_SsKeyframe*	findLeftKey(int time);

};



#endif
