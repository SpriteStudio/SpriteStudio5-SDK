#ifndef __SAMPLESCENE__
#define __SAMPLESCENE__

#include "ssHelper.h"
//#include "./player/ssplayer_animedecode.h"

class SsAnimeDecoder;
class SsCellMapList;


///サンプル用のシーンクラス　
class SampleScene : public tkScene
{
public:
	struct AnimePackSelecterType;
	bool		isLoop;


private:

	SsAnimeDecoder*	m_player;
	SsCellMapList*	m_cellmap;


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
		destroy();
	}

	virtual void	draw();
	virtual void	update(double delta);
	virtual void	init();
	virtual void	destroy();

	void	ProjectFileLoad();

	void	UIRebuild();

	void	AnimePackSelecterRelease();
	void	ChangeAnimation( int packIndex , int animeIndex );


	void	AnimePlay(){ m_isAnimeAutoPlay = true; }
	void	AnimePause(){ m_isAnimeAutoPlay = false; }
	void	AnimeReset();

};


#endif
