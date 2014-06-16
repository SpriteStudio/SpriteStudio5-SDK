#include "ssplayer_animedecode.h"
#include "bind_SsKeyFrame.h"


SsColorAnime*	Bind_SsKeyframe::getColorAnime()
{
	SsColorAnime* a = new SsColorAnime();
	GetSsColorValue( this->bind_inst , *a );
	
	return a;
}


SsVertexAnime*	Bind_SsKeyframe::getVertexAnime()
{
	SsVertexAnime* a = new SsVertexAnime();
	GetSsVertexAnime( this->bind_inst , *a );
	
	return a;
}


SsRefCell*		Bind_SsKeyframe::getRefCell()
{
	SsRefCell* cell = new SsRefCell();
//	GetSsVertexAnime( this->bind_inst , *a );
	GetSsRefCell( this->bind_inst , *cell );
	return cell;
}


SsUserDataAnime*	Bind_SsKeyframe::getUserDataAnime()
{
	SsUserDataAnime*	udat = new SsUserDataAnime();

	GetSsUserDataAnime( this->bind_inst , *udat );
	
	return udat;
}
