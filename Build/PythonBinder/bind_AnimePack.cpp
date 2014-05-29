//
#include "package_SpriteStudio.h"
#include "bind_AnimePack.h"
#include "ssplayer_animedecode.h"

Bind_SsAnimePack::Bind_SsAnimePack()
 : m_animepack(0) , m_cellmap(0)
{
	
}


bool Bind_SsAnimePack::debug()
{ 
	if ( m_animepack )
	{
		PYDEBUG_PRINTF( "AnimePack Name = %s" , m_animepack->name.c_str() );
		PYDEBUG_PRINTF( "AnimePack Anime num = %d" , m_animepack->animeList.size() );		
		PYDEBUG_PRINTF( "AnimePack Model part num = %d" , m_animepack->Model.partList.size() );
		PYDEBUG_PRINTF( "AnimePack canvas size = ( %f , %f )" ,
			m_animepack->settings.canvasSize.x ,
			m_animepack->settings.canvasSize.y );

		PYDEBUG_PRINTF( "AnimePack FPS = %d " , m_animepack->settings.fps );
		PYDEBUG_PRINTF( "AnimePack Frames = %d " , m_animepack->settings.frameCount );
		PYDEBUG_PRINTF( "AnimePack pivot = ( %d , %d )" , 
			m_animepack->settings.pivot.x ,
			m_animepack->settings.pivot.y );
	}

	return true; 
}

Bind_SsAnimeDecoder*	Bind_SsAnimePack::getAnimeDecoderByName( const char* animename )
{
	if ( this->m_animepack )
	{
		std::vector<SsAnimation*>& animeList = this->m_animepack->animeList;

		for ( std::vector<SsAnimation*>::iterator itr = animeList.begin() ; 
			itr != animeList.end() ; itr ++ )
		{
			if ( (*itr)->name == animename )
			{				
				SsAnimation* anime = (*itr);		
				Bind_SsAnimeDecoder* ad = new Bind_SsAnimeDecoder();


				SsModel* model = &m_animepack->Model;
				if ( m_cellmap ) delete m_cellmap;
				m_cellmap = new SsCellMapList();
				m_cellmap->set( m_proj , m_animepack );

				//インスタンスの寿命管理を考えること
				SsAnimeDecoder* player = new SsAnimeDecoder();
				ad->m_decoder = player;
				player->setAnimation( model , anime , m_cellmap , this->m_proj );

				return ad;
			}
		}
	}

	return 0;
}

Bind_SsAnimeDecoder*	Bind_SsAnimePack::getAnimeDecoderByIndex( int index )
{
	if ( this->m_animepack )
	{
		if ( (size_t)index < this->m_animepack->animeList.size() )
		{
			SsAnimation* anime = this->m_animepack->animeList[index];
			Bind_SsAnimeDecoder* ad = new Bind_SsAnimeDecoder();


			SsModel* model = &m_animepack->Model;
			if ( m_cellmap ) delete m_cellmap;
			m_cellmap = new SsCellMapList();
			m_cellmap->set( m_proj , m_animepack );

			//インスタンスの寿命管理を考えること
			SsAnimeDecoder* player = new SsAnimeDecoder();
			ad->m_decoder = player;
			player->setAnimation( model , anime , m_cellmap , this->m_proj );

			return ad;
		}
	}

	return 0;
}
