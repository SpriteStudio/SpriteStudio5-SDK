#include "ssloader_ssee.h"
#include "ssstring_uty.h"

SsEffectFile*	ssloader_ssee::Load(const std::string& filename )
{

	SsString _basepath = "";

	
	SsEffectFile* effectFile = new SsEffectFile();

	XMLDocument xml;
	if ( XML_SUCCESS == xml.LoadFile( filename.c_str() ) )
	{
		SsXmlIArchiver ar( xml.GetDocument() , "SpriteStudioEffect" );
		effectFile->__Serialize( &ar );
		//effectFile->fname = path2file( filename );
	}else{
		delete effectFile;
		effectFile = 0;
	}

	return effectFile;

}


void	SsEffectModel::EffectNodeLoader(ISsXmlArchiver* ar)
{

	//SsEffectNode* root;

	

}