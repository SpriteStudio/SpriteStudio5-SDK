#include "ssloader_ssae.h"


SsAnimePack*	ssloader_ssae::Load(const std::string& filename )
{
#ifdef USE_SSARCHIVER
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
#endif
	return 0;
}


SsAnimation*	SsAnimePack::findAnimation(SsString& name)
{

	for ( std::vector<SsAnimation*>::iterator itr = animeList.begin()
		; itr != animeList.end() ; ++itr )
	{
		if ( (*itr)->name == name )
		{
			return (*itr);
		}
	}
	return 0;
}