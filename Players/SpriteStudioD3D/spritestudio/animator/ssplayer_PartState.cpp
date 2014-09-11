#include "stdafx.h"

#include "../loader/ssloader.h"
#include "ssplayer_animedecode.h"
#include "ssplayer_PartState.h"

SsPartState::SsPartState() : refAnime(0) {init();}

SsPartState::~SsPartState(){
	destroy();
}


void	SsPartState::destroy()
{
	if ( refAnime )	
	{
		delete refAnime;
		refAnime = 0;
	}
}

void	SsPartState::init()
{
	memset( vertices , 0 , sizeof( vertices ) );
	memset( colors , 0 , sizeof( colors ) );
	memset( uvs , 0 , sizeof( uvs ) );
	memset( matrix , 0 , sizeof( matrix ) );
	//cell = 0;
	position = SsVector3( 0.0f , 0.0f, 0.0f );
	rotation = SsVector3( 0.0f , 0.0f , 0.0f );
	scale = SsVector2( 1.0f , 1.0f );

	anchor = SsVector2( 0 , 0 );
	size = SsVector2( 0 , 0 );

	alpha = 1.0f;
	prio = 0;
	hFlip = false;
	vFlip = false;
	hide = false;
	imageFlipH = false;
	imageFlipV = false;

	uvRotation = 0;
	uvTranslate = SsVector2( 0 , 0 );
	pivotOffset = SsVector2( 0 , 0 );
	uvScale = SsVector2( 1 , 1 );

	is_color_blend = false;
	is_vertex_transform = false;
	inheritRates = 0; 
		
}


