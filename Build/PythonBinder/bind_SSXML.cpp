//
#include "bind_SSXML.h"




SSXML::SSXML() 
 : m_bind_project(0) { 
	puts("construct SSXML"); 
}

bool	SSXML::Load( const char* name )
{
	if  ( m_bind_project != 0 ) delete m_bind_project;

	SsProject*	m_project = ssloader_sspj::Load( name );

	m_bind_project = new Bind_SsProject();
	m_bind_project->m_project = m_project;

	if ( m_project==0 ) return false;

	m_bind_project->createAnimepack();

	return true;
}


#define DEBUG_OUT(x) sprintf( buffer , x ); puts( buffer );


void Bind_SsProject::createAnimepack()
{
	size_t num = m_project->getAnimePackNum();
	SsAnimePackList& list = m_project->getAnimePackList();

	for ( size_t i = 0 ; i < num ; i++ )
	{
		Bind_SsAnimePack animepack;
		animepack.m_animepack = list[i];

		animepack.m_proj = m_project;
		m_animepacklist.push_back( animepack );
	}
}

Bind_Cellmap*	Bind_SsProject::getCellMapAt( int at )
{

	//this->m_project->getCellMapList()[at];
	SsCellMap* cellmap = this->m_project->getCellMap( at );

	Bind_Cellmap* bcell = new Bind_Cellmap();
	bcell->bind( cellmap );

	return bcell;
}


Bind_Cellmap*	Bind_SsProject::getCellMapFromName( const char* name )
{

	SsString str = name;
	SsCellMap* cellmap = this->m_project->findCellMap( str );
	if ( cellmap == 0)
	{

		str+=".ssce";
		cellmap = this->m_project->findCellMap( str );
	}



	Bind_Cellmap* bcell = new Bind_Cellmap();
	bcell->bind( cellmap );

	return bcell;

}


Bind_SsProject::Bind_SsProject()
{

}

bool Bind_SsProject::debug()
{
//	puts( "SsProject");
	if ( m_project )
	{
		//アニメパックの数
		PYDEBUG_PRINTF( "AnimePack Num = %d" , m_project->getAnimePackNum() );
		PYDEBUG_PRINTF( "CellMap Num = %d" , m_project->getCellMapNum() );

	}

	return true;
}



