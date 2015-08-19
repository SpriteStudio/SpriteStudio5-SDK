#ifndef __WTOBJECT__
#define __WTOBJECT__

#include "wtAtom.h"
#include "wtafx.h"

class wtObjectBase : public wtAtom
{
public:
	wtObjectBase(){}
	virtual ~wtObjectBase(){}

	virtual const wtString getObjectType(){ return "wtObject"; }

};

typedef treeitem<wtObjectBase> wtObject;


#endif
