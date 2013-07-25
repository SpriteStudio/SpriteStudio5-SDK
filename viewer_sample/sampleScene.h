#ifndef __SAMPLESCENE__
#define __SAMPLESCENE__

//#include <GLFW/glfw3.h>
#include "./helper/tkScene.h"
#include "./player/ssplayer_player.h"


///サンプル用のシーンクラス　
class SampleScene : public tkScene
{
public:
	struct AnimePackSelecterType;


private:
//	GLuint tex;
	SsPlayer*	m_player;

	SsProject*	m_proj;
	int			m_select_animepack_index;
	int			m_select_anime_index;

	std::vector<AnimePackSelecterType*>	AnimePackSelecter;
	SsVector2	m_animeOffsetpos;
	int			m_nowPlayFrame;
	double		m_nowPlayFrameD;


	bool		m_isAnimeAutoPlay;

	//UIへの情報表示用
	int			m_InfoAnimeEndFrame;
	int			m_InfoAnimeFps;

	float		m_Zoom;
	float		m_Speed;



public:
	SampleScene() : m_isAnimeAutoPlay (false) {}
	virtual ~SampleScene()
	{
		AnimePackSelecterRelease();
	}

	virtual void	draw();
	virtual void	update();
	virtual void	init();

	void	ProjectFileLoad();

	void	UIRebuild();

	void	AnimePackSelecterRelease();
	void	ChangeAnimation( int packIndex , int animeIndex );


	void	AnimePlay(){ m_isAnimeAutoPlay = true; }
	void	AnimePause(){ m_isAnimeAutoPlay = false; }
	void	AnimeReset(){ 
		m_isAnimeAutoPlay = false; 
		m_nowPlayFrame = 0;
		m_nowPlayFrameD = 0;
	}




};


#endif
