//
#include "bind_SSXML.h"




SSXML::SSXML() 
 : m_bind_project(0) { 
	puts("construct SSXML"); 
}

bool	SSXML::Load( const char* name )
{
	if  ( m_bind_project != 0 ) delete m_bind_project;

//	m_project = new SsProject();
	SsProject*	m_project = ssloader_sspj::Load( name );

	m_bind_project = new Bind_SsProject();
	m_bind_project->m_project = m_project;

	if ( m_project==0 ) return false;

//	animepack.m_animepack = 


	return true;
}


#define DEBUG_OUT(x) sprintf( buffer , x ); puts( buffer );

bool Bind_SsProject::debug()
{
//	puts( "SsProject");
	if ( m_project )
	{
		//アニメパックの数
		PYDEBUG_PRINTF( "AnimePack Num = %d" , m_project->getAnimePackNum() );
		PYDEBUG_PRINTF( "CellMap Num = %d" , m_project->getCellMapNum() );

		size_t num = m_project->getAnimePackNum();
		SsAnimePackList& list = m_project->getAnimePackList();

		for ( size_t i = 0 ; i < num ; i++ )
		{
			Bind_SsAnimePack animepack;
			animepack.m_animepack = list[i];
			m_animepacklist.push_back( animepack );
		}

	}

	return true;
}



