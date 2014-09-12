﻿#include "stdafx.h"
#include "ssloader_sspj.h"
#include "ssloader_ssae.h"
#include "ssloader_ssce.h"
#include "ssstring_uty.h"
#include "../Helper/DebugPrint.h"




SsString	SsProject::getSsceBasepath(){ 
	return getFullPath( m_proj_filepath , settings.cellMapBaseDirectory );
}


SsString	SsProject::getSsaeBasepath()
{ 
	return getFullPath( m_proj_filepath , settings.animeBaseDirectory );
}


SsString	SsProject::getImageBasepath()
{
	return getFullPath( m_proj_filepath , settings.imageBaseDirectory );
}

SsProject::~SsProject()
{
	for ( SsAnimePackList::iterator itr = animeList.begin() ; 
		itr != animeList.end() ; itr ++ ) delete (*itr);	
	for ( SsSsCellMapList::iterator itr = cellmapList.begin() ; 
		itr != cellmapList.end() ; itr ++ ) delete (*itr);	
}


SsAnimePack*		SsProject::findAnimationPack( SsString& animePackName )
{
	for ( SsAnimePackList::iterator itr = animeList.begin()
		; itr != animeList.end() ; ++itr )
	{
		if ( (*itr)->name == animePackName )
		{
			return (*itr);
		}
	}

	return 0;
}



SsAnimation*	SsProject::findAnimation( SsString& animePackName , SsString& AnimeName )
{
	SsAnimePack* p = findAnimationPack( animePackName );
	if ( p )
	{
		return p->findAnimation(AnimeName);
	}

	return 0;
}



SsProject*	ssloader_sspj::Load(const std::string& filename )
{

	tinyxml2::XMLDocument xml;
	if ( XML_SUCCESS == xml.LoadFile( filename.c_str() ) )
	{
		SsXmlIArchiver ar( xml.GetDocument() , "SpriteStudioProject" );

		SsProject* proj = new SsProject();
		proj->__Serialize( &ar );
		std::string project_filepath = path2dir( filename );
		proj->setFilepath( project_filepath );

		//アニメーションリストを元に読み込みます。
		for ( size_t i = 0 ;i < proj->getAnimePackNum() ; i++ )
		{

			SsString ssaepath = proj->getAnimePackFilePath(i);
			SsAnimePack* anime = ssloader_ssae::Load( ssaepath );
			if ( anime )
			{
				proj->animeList.push_back( anime );
			}else{
				//エラー
				DEBUG_PRINTF( "Animation load error : %s" , ssaepath.c_str() );
				delete proj;
				return 0;
			}
		}

		//セルマップリストを元に読み込みます。
		for ( size_t i = 0 ;i < proj->getCellMapNum() ; i++ )
		{
			SsString sscepath = proj->getCellMapFilePath(i);
			SsCellMap* cell = ssloader_ssce::Load( sscepath );
			if ( cell )
			{
				proj->cellmapList.push_back( cell );
			}else{
				//エラー
				DEBUG_PRINTF( "Cellmap load error : %s" , sscepath.c_str() );
				delete proj;
				return 0;
			}
		}

		return proj;
	}	

	return 0;
}

SsCellMap* SsProject::findCellMap( SsString& str )
{
	for ( SsSsCellMapListItr itr = cellmapList.begin() ; 
		itr != cellmapList.end() ; itr ++) 
	{
//		SsString _name = (*itr)->name;
		//_name+=".ssce";
		//sspjの参照名とXML無いのnameタグが一致していないケースがあったのでファイル名で取得
		SsString _name = (*itr)->fname;

		if ( _name == str )
		{
			return (*itr);
		}
	}
	return 0;
}

SsCellMap* SsProject::getCellMap( int index )
{
	return cellmapList[index];
}