#include "ssplayer_animedecode.h"
#include "bind_SsAttribute.h"


Bind_SsKeyframe*	Bind_SsAttribute::firstKey()
{ 
	SsKeyframe* key = (SsKeyframe*)bind_inst->firstKey();
	if ( key == 0 ) return new Bind_SsKeyframe( 0 , SsAttributeKind::invalid ) ;
	prevKey = key;
	return new Bind_SsKeyframe( key , this->bind_inst->tag ) ;
}


Bind_SsKeyframe*	Bind_SsAttribute::nextKey()
{
	if ( prevKey == 0 ) return 0;

	SsKeyframe* key = (SsKeyframe*)bind_inst->findRightKey( prevKey->time );
	if ( key == 0 ) return new Bind_SsKeyframe( 0 , SsAttributeKind::invalid ) ;
	prevKey = key;

//	PYDEBUG_PRINTF( "prevKeytime = %d" , prevKey->time);
	return new Bind_SsKeyframe( key , this->bind_inst->tag ) ;

}


Bind_SsKeyframe*	Bind_SsAttribute::findRightKey(int time)
{
	SsKeyframe* key = (SsKeyframe*)bind_inst->findRightKey( time );
	if ( key == 0 ) return new Bind_SsKeyframe( 0 , SsAttributeKind::invalid ) ;
	prevKey = key;
	return new Bind_SsKeyframe( key , this->bind_inst->tag ) ;

}


Bind_SsKeyframe*	Bind_SsAttribute::findLeftKey(int time)
{
	SsKeyframe* key = (SsKeyframe*)bind_inst->findLeftKey( time );
	if ( key == 0 ) return new Bind_SsKeyframe( 0 , SsAttributeKind::invalid ) ;
	prevKey = key;
	return new Bind_SsKeyframe( key , this->bind_inst->tag ) ;

}
