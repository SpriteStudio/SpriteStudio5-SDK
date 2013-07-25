#include "ssloader_ssce.h"
#include "ssstring_uty.h"



SsCellMap*	ssloader_ssce::Load(const std::string& filename )
{
	SsString _basepath = "";

	
	SsCellMap* cellmap = new SsCellMap();

	XMLDocument xml;
	if ( XML_SUCCESS == xml.LoadFile( filename.c_str() ) )
	{
		SsXmlIArchiver ar( xml.GetDocument() , "SpriteStudioCellMap" );
		cellmap->__Serialize( &ar );
	}else{
		delete cellmap;
		cellmap = 0;
	}

	return cellmap;
}



