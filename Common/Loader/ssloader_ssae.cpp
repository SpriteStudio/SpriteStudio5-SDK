#include "ssloader_ssae.h"


SsAnimePack*	ssloader_ssae::Load(const std::string& filename )
{

	SsAnimePack* anime = new SsAnimePack();

	XMLDocument xml;
	if ( XML_SUCCESS == xml.LoadFile( filename.c_str() ) )
	{
		SsXmlIArchiver ar( xml.GetDocument() , "SpriteStudioAnimePack" );
		anime->__Serialize( &ar );
	}else{
		delete anime;
		anime = 0;
	}

	return anime;
}

