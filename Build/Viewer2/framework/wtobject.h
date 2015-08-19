#ifndef __WTOBJECT__
#define __WTOBJECT__

#include "wtAtom.h"
#include "wtafx.h"

class wtObjectBase
{
public:
	wtObjectBase(){}
	virtual ~wtObjectBase(){}

	virtual const wtString getObjectType(){ return "wtObject"; }

};


#endif
