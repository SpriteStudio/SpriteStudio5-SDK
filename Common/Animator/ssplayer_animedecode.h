#ifndef __SSPLAYER_ANIMEDECODE__
#define __SSPLAYER_ANIMEDECODE__

#include "../loader/ssloader.h"
#include "../Helper/ssHelper.h"


#include "ssplayer_types.h"
#include "ssplayer_cellmap.h"
#include "ssplayer_PartState.h"



class SsAnimeDecoder;
class SsCelMapLinker;


//パーツとアニメを関連付ける
typedef std::pair<SsPart*,SsPartAnime*>	SsPartAndAnime;


//パーツのソート順
class SsPartStateLess
{
public:
	bool operator()(const SsPartState* lhs, const SsPartState* rhs) const
	{
		if (lhs->prio == rhs->prio)
			return lhs->index < rhs->index;
		return lhs->prio < rhs->prio;
	}
};



class SsAnimeDecoder
{
public:

private:
	std::vector<SsPartAndAnime>		part_anime;
	std::map<SsString,SsPartAnime*> PartAnimeDic;

	SsCellMapList*				curCellMapManager;
	SsPartState*				partState;			///パーツの現在の状態が格納されています。
	std::list<SsPartState*>		sortList;			///ソート状態

	float	nowPlatTime;
	int		curAnimeEndFrame;
	int		curAnimeFPS;

private:
	void	updateState( int nowTime , SsPart* part , SsPartAnime* part_anime , SsPartState* state );
	void	updateMatrix(SsPart* part , SsPartAnime* anime , SsPartState* state);
	void	updateVertices(SsPart* part , SsPartAnime* anime , SsPartState* state);


public:
	SsAnimeDecoder();
	virtual ~SsAnimeDecoder()
	{
		if ( curCellMapManager )
			delete curCellMapManager;

		if ( partState )
			delete [] partState;
	}

	virtual void	update();
	virtual void	draw();


	void	setAnimation( SsModel*	model , SsAnimation* anime , SsCellMapList* cellmap );

	void	setProject( SsProject* proj , int packIndex = 0, int animeIndex = 0){}
	void	changeAnimation(int packIndex = 0, int animeIndex = 0){}


	void	setPlayFrame( float time ) { nowPlatTime = time; }
	int		getAnimeEndFrame() { return curAnimeEndFrame; }
	int		getAnimeFPS() { return curAnimeFPS; }		

};



#endif
