#include "myscene.h"

#include "ssHelper.h"
#include "ssplayer_animedecode.h"

//パラメータ等をウインドウへ表示するために使用します。
#include <AntTweakBar.h>


extern double _time;
extern unsigned char cubeColor;



extern TwBar *g_twbar;
extern float bgColor[];
// int g_particle_num;
// int g_particle_draw_num;


struct SampleScene::AnimePackSelecterType
{
	std::string		name;
	int				packindex;
	int				animeindex;
	SampleScene*	myclass;
};

void	SampleScene::destroy()
{
	delete m_player;
//	delete m_cellmap;	/// プレイヤー側で削除する

	m_player = 0;
//	m_cellmap = 0; 
	AnimePackSelecterRelease();

}


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
	float frameDelta = 0;
	if ( m_player )
	{
		m_InfoAnimeFps = m_player->getAnimeFPS();
		m_InfoAnimeEndFrame = m_player->getAnimeEndFrame();
	}
	if ( m_isAnimeAutoPlay )
	{
		double anime_fps = (double)m_player->getAnimeFPS();
		double frameper_sec = (1.0 / anime_fps) ;
		frameDelta = (delta*m_Speed / frameper_sec );

		m_nowPlayFrameD+= frameDelta;
		m_nowPlayFrame = (int)m_nowPlayFrameD;

		if ( isLoop )
		{
			if ( m_player->getAnimeEndFrame() < (int)m_nowPlayFrame )
			{
				m_nowPlayFrame = 0;
				m_nowPlayFrameD = 0;
				sceneLoop++;
				//seedOffset
				m_player->setSeedOffset( sceneLoop );
				//m_player->restart();
			}
		}
	}
	static float backframe = 0;
	
	m_player->setPlayFrame( (float)m_nowPlayFrame );
	m_player->update(frameDelta);


	backframe = m_nowPlayFrame;

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

void TW_CALL AnimeResetCB(void *clientData)
{
	SampleScene* scene = (SampleScene*)clientData;

	scene->AnimeReset();

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


void	SampleScene::AnimeReset(){ 

	m_isAnimeAutoPlay = false; 
	m_nowPlayFrame = 0;
	m_nowPlayFrameD = 0;
	m_player->reset();
	sceneLoop = 0;
}


void	SampleScene::UIRebuild()
{

    TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLFW and OpenGL.' "); // Message added to the help bar.
	TwAddButton( g_twbar , "Load .sspj" ,ProjFileLoadCB , this , "" );

	TwAddSeparator( g_twbar , "sep1" , "" );
	TwAddButton( g_twbar , "Play" , AnimePlayCB , this , "" );
	TwAddButton( g_twbar , "Pause" , AnimePauseCB , this , "" );
	TwAddButton( g_twbar , "Reset" , AnimeResetCB , this , "" );

//	TwAddButton( g_twbar , "Loop" , TW_TYPE_BOOL8 , this , "" );
    TwAddVarRW(g_twbar, "Loop", TW_TYPE_BOOL32, &isLoop, "");

    //TwAddVarRW(g_twbar, "zoom", TW_TYPE_FLOAT, &m_Zoom, " min='0.1' max='10' step=0.1 ");
    TwAddVarRW(g_twbar, "speed", TW_TYPE_FLOAT, &m_Speed, " min='0'max='5' step=0.1 ");
    TwAddVarRW(g_twbar, "frame", TW_TYPE_INT32, &m_nowPlayFrame, " min='0' ");
//    TwAddVarRW(g_twbar, "particle", TW_TYPE_INT32, &g_particle_num, " min='0' ");
//    TwAddVarRW(g_twbar, "particle draw", TW_TYPE_INT32, &g_particle_draw_num, " min='0' ");

	

    TwAddVarRW(g_twbar, "Loop num", TW_TYPE_INT32, &sceneLoop, " group='Anime Info' ");
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

	//アニメパックを選択
	SsAnimePack* animepack = m_proj->getAnimePackList()[packIndex]; 

	//アニメパックのパーツ構造を取得
	SsModel* model = &animepack->Model;

	//アニメパック内のアニメーションを選択
	SsAnimation* anime = animepack->animeList[animeIndex]; 

	//セルマップ情報を作成
	m_cellmap->set( m_proj , animepack );

	//パーツ構造　アニメーション　セルマップからアニメ再生情報を作成する
	if ( m_cellmap->size() == 0 )
	{
		DEBUG_PRINTF( "error : cellmap array size == 0" );
//		abort();	//インスタンスのみのアニメでは使用しているセルが０があり得るためコメント
	}
	m_player->setAnimation( model , anime , m_cellmap , m_proj );

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
	m_cellmap = new SsCellMapList();
	

}


void	SampleScene::ProjectFileLoad()
{
    XPFileOpenDlg dlg;
    if ( dlg.Show() )
	{    	

		if ( m_proj ) delete m_proj;

		//sspjファイルを読み込む
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

		ChangeAnimation( 0 , 0 );

	}
}





