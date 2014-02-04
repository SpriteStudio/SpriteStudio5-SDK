#ifndef __BIND_CELLMAP__
#define __BIND_CELLMAP__

#include "package_SpriteStudio.h"

class Bind_Cell : public myPyBinder<SsCell>
{
public:
	Bind_Cell(){}
	virtual ~Bind_Cell(){}

	const char* name(){ return bind_inst->name.c_str();}
	SsPoint2 pos(){ return bind_inst->pos;}
	SsPoint2 size(){ return bind_inst->size;}
	SsPoint2 pivot(){ return bind_inst->pivot;}
	bool	rotated(){ return bind_inst->rotated;}


};

class Bind_Cellmap : public myPyBinder<SsCellMap>
{
public:
	Bind_Cellmap(){}
	virtual ~Bind_Cellmap(){}

	const char* name(){ return bind_inst->name.c_str();}
	const char* imagePath(){ return bind_inst->imagePath.c_str();}
	SsPoint2 pixelSize(){ return bind_inst->pixelSize;}
	bool overrideTexSettings(){ return bind_inst->overrideTexSettings;}
	int  wrapMode(){ return (int)bind_inst->wrapMode;}	
	int  filterMode(){ return (int)bind_inst->filterMode;}	

	int	getCellNum()
	{
		return (int)bind_inst->cells.size();
	}

	Bind_Cell*	getCell(int index);

};

#endif

