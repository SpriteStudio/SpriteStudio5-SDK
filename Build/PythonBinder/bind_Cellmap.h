#ifndef __BIND_CELLMAP__
#define __BIND_CELLMAP__

#include "package_SpriteStudio.h"


#include "IsshTexture.h"
#include "ssplayer_cellmap.h"



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

class ISSTexture;
class Bind_SsTexture : public myPyBinder<ISSTexture>
{
public:
	Bind_SsTexture(){}
	virtual ~Bind_SsTexture(){}

	int	getWidth(){ return bind_inst->getWidth(); }
	int	getHeight(){ return bind_inst->getWidth(); }
	const char*	getFilename(){ return bind_inst->getFilename(); }

};

struct SsCellValue;
class Bind_SsCellValue : public myPyBinder<SsCellValue>
{
public:
	Bind_SsCellValue(){}
	virtual ~Bind_SsCellValue(){}

	SsVector2	getuvs(int index) { return bind_inst->uvs[index]; }
	BIND_RETURN_PROPEX( int ,wrapMode );
	BIND_RETURN_PROPEX( int ,filterMode );

	BIND_NEW_PROP( Bind_Cell , cell );
	BIND_NEW_PROP( Bind_SsTexture , texture );

};


#endif

