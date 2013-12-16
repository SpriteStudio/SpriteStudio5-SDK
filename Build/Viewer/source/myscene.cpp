#include "myscene.h"

#include "ssHelper.h"
#include "ssplayer_animedecode.h"

//パラメータ等をウインドウへ表示するために使用します。
#include <AntTweakBar.h>

extern double _time;
extern unsigned char cubeColor;



extern TwBar *g_twbar;
extern float bgColor[];




struct SampleScene::AnimePackSelecterType
{
	std::string		name;
	int				packindex;
	int				animeindex;
	SampleScene*	myclass;
};


void	SampleScene::draw()
{
/*
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
*/
	m_player->draw();
	
}

void	SampleScene::update(double delta)
{

	if ( m_player )
	{
		m_InfoAnimeFps = m_player->getAnimeFPS();
		m_InfoAnimeEndFrame = m_player->getAnimeEndFrame();
	}
	if ( m_isAnimeAutoPlay )
	{
		double anime_fps = (double)m_player->getAnimeFPS();

		double frameper_sec = (1.0 / anime_fps) ;
		m_nowPlayFrameD+= (delta*m_Speed / frameper_sec );
		m_nowPlayFrame = (int)m_nowPlayFrameD;

		if ( m_player->getAnimeEndFrame() < (int)m_nowPlayFrame )
		{
			m_nowPlayFrame = 0;
			m_nowPlayFrameD = 0;
		}
	}

	m_player->setPlayFrame( (float)m_nowPlayFrame );
	m_player->update();

}


void TW_CALL ProjFileLoadCB(void *clientData)
{
	SampleScene* scene = (SampleScene*)clientData;
	scene->ProjectFileLoad();
}

void TW_CALL AnimePlayCB(void *clientData)
{
	SampleScene* scene = (SampleScene*)clientData;
	scene->AnimePlay();
}
void TW_CALL AnimePauseCB(void *clientData)
{
	SampleScene* scene = (SampleScene*)clientData;

	scene->AnimePause();

}


void TW_CALL AnimeSelectCB(void *clientData)
{
	SampleScene::AnimePackSelecterType* data = (SampleScene::AnimePackSelecterType*)clientData;
	data->myclass->AnimeReset();

	
	data->myclass->ChangeAnimation( data->packindex ,  data->animeindex );

	data->myclass->AnimePlay();

}

void	SampleScene::AnimePackSelecterRelease()
{
	for ( size_t i = 0 ; i < AnimePackSelecter.size() ; i++ )
	{
		delete AnimePackSelecter[i];
	}
	AnimePackSelecter.clear();
}

void	SampleScene::UIRebuild()
{

    TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLFW and OpenGL.' "); // Message added to the help bar.
	TwAddButton( g_twbar , "Load .sspj" ,ProjFileLoadCB , this , "" );

	TwAddSeparator( g_twbar , "sep1" , "" );
	TwAddButton( g_twbar , "Play" , AnimePlayCB , this , "" );
	TwAddButton( g_twbar , "Pause" , AnimePauseCB , this , "" );
    //TwAddVarRW(g_twbar, "zoom", TW_TYPE_FLOAT, &m_Zoom, " min='0.1' max='10' step=0.1 ");
    TwAddVarRW(g_twbar, "speed", TW_TYPE_FLOAT, &m_Speed, " min='0'max='5' step=0.1 ");
    TwAddVarRW(g_twbar, "frame", TW_TYPE_INT32, &m_nowPlayFrame, " min='0' ");
	
    TwAddVarRW(g_twbar, "Endframe", TW_TYPE_INT32, &m_InfoAnimeEndFrame, " group='Anime Info' ");
    TwAddVarRW(g_twbar, "FPS", TW_TYPE_INT32, &m_InfoAnimeFps, " group='Anime Info' ");

//    TwAddVarRW(g_twbar, "pos-X", TW_TYPE_FLOAT, &m_animeOffsetpos.x, " group='Position' min='-1000' max ='1000' ");
//    TwAddVarRW(g_twbar, "pos-Y", TW_TYPE_FLOAT, &m_animeOffsetpos.y, " group='Position' min='-1000' max ='1000' ");

	TwAddSeparator( g_twbar , "sep2" , "" );
    TwAddVarRW(g_twbar, "bgColor", TW_TYPE_COLOR3F, &bgColor, " label='Background color' ");

	TwAddSeparator( g_twbar , "sep3" , "" );

}


void	SampleScene::ChangeAnimation( int packIndex , int animeIndex )
{
	m_player->changeAnimation( packIndex , animeIndex );
}



void	SampleScene::init()
{
	m_select_animepack_index = 0;
	m_select_anime_index = 0;
	g_twbar = 0;
	m_animeOffsetpos.x = 0;
	m_animeOffsetpos.y = 0;
	m_nowPlayFrame = 0;
	m_proj = 0;

	m_nowPlayFrame = 0;
	m_nowPlayFrameD = 0;

	m_InfoAnimeEndFrame = 0;
	m_InfoAnimeFps = 0;

	m_Zoom = 1.0f;
	m_Speed = 1.0f;

	
    g_twbar = TwNewBar("Main Panel");
	UIRebuild();


	m_player = new SsAnimeDecoder();
	

}


void	SampleScene::ProjectFileLoad()
{
    XPFileOpenDlg dlg;
    if ( dlg.Show() )
	{    	

		if ( m_proj ) delete m_proj;
		m_proj = ssloader_sspj::Load( dlg.getFilePath() );

		//UIの作り直し
		TwRemoveAllVars( g_twbar );
		UIRebuild();
		
		AnimePackSelecterRelease();
		SsAnimePackList& lpack =  m_proj->getAnimePackList();
		for ( size_t i = 0 ; i < lpack.size() ; i++ )
		{
			for ( size_t n = 0 ; n < lpack[i]->animeList.size() ; n++ )
			{
				AnimePackSelecterType* temp = new AnimePackSelecterType();

				std::string disp_name = lpack[i]->name + " : " + lpack[i]->animeList[n]->name;

				temp->packindex = i;
				temp->animeindex = n;
				temp->myclass = this;
				temp->name = disp_name;
				AnimePackSelecter.push_back( temp );

				TwAddButton( g_twbar , disp_name.c_str() , AnimeSelectCB , temp , "group='Animetion'" );
			}
		}

		delete m_player;
		m_player = new SsAnimeDecoder();
        m_player->setProject( m_proj );

	}
}





