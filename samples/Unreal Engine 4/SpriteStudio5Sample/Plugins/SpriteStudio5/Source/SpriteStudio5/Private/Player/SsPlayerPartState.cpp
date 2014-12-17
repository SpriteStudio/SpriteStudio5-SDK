#include "SpriteStudio5PrivatePCH.h"
#include "SsPlayerPartState.h"

#include "SsPlayerAnimedecode.h"

FSsPartState::FSsPartState() : RefAnime(NULL) {Init();}

FSsPartState::~FSsPartState(){
	Destroy();
}


void	FSsPartState::Destroy()
{
	if(RefAnime)
	{
		delete RefAnime;
		RefAnime = 0;
	}
}

void	FSsPartState::Init()
{
	memset( Vertices , 0 , sizeof( Vertices ) );
	memset( Colors , 0 , sizeof( Colors ) );
	memset( Uvs , 0 , sizeof( Uvs ) );
	memset( Matrix , 0 , sizeof( Matrix ) );
	//cell = 0;
	Position = FVector( 0.0f , 0.0f, 0.0f );
	Rotation = FVector( 0.0f , 0.0f , 0.0f );
	Scale = FVector2D( 1.0f , 1.0f );

	Anchor = FVector2D( 0 , 0 );
	Size = FVector2D( 1.f , 1.f );

	Alpha = 1.0f;
	Prio = 0;
	HFlip = false;
	VFlip = false;
	Hide = false;
	ImageFlipH = false;
	ImageFlipV = false;

	UvRotation = 0;
	UvTranslate = FVector2D( 0 , 0 );
	PivotOffset = FVector2D( 0 , 0 );
	UvScale = FVector2D( 1 , 1 );

	IsColorBlend = false;
	IsVertexTransform = false;
	InheritRates = 0; 
		
}


