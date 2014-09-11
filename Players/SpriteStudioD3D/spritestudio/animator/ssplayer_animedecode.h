#ifndef __SSPLAYER_ANIMEDECODE__
#define __SSPLAYER_ANIMEDECODE__

#include "../loader/ssloader.h"
#include "../helper/isshtexture.h"

#include "ssplayer_types.h"
#include "ssplayer_cellmap.h"
#include "ssplayer_PartState.h"

#define foreach(T, c, i) for(T::iterator i = c.begin(); i!=c.end(); ++i)


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

	///パーツ情報とパーツアニメーションを結びつけアレイにしたもの
	std::vector<SsPartAndAnime>		partAnime;

	///パーツ名からアニメ情報をとるために使うもし、そういった用途が無い場合はローカル変数でも機能する
	std::map<SsString,SsPartAnime*> partAnimeDic;
	
	SsCellMapList*					curCellMapManager;///アニメに関連付けられているセルマップ

	SsPartState*					partState;			///パーツの現在の状態が格納されています。
	std::list<SsPartState*>			sortList;			///ソート状態

	float			nowPlatTime;
	float			frameDelta;
	int				curAnimeEndFrame;
	int				curAnimeFPS;
	SsAnimation*	curAnimation;

private:
	void	updateState( int nowTime , SsPart* part , SsPartAnime* part_anime , SsPartState* state );
	void	updateInstance( int nowTime , SsPart* part , SsPartAnime* part_anime , SsPartState* state );
	void	updateMatrix(SsPart* part , SsPartAnime* anime , SsPartState* state);
	void	updateVertices(SsPart* part , SsPartAnime* anime , SsPartState* state);

    int		CalcAnimeLabel2Frame(const SsString& str, int offset );
	int		findAnimetionLabel(const SsString& str);


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

	void	setAnimation( SsModel*	model , SsAnimation* anime , SsCellMapList* cellmap , SsProject* sspj=0 );

	void	setPlayFrame( float time ) { nowPlatTime = time; }
	int		getAnimeEndFrame() { return curAnimeEndFrame; }
	int		getAnimeFPS() { 
		return curAnimeFPS; }		


	std::list<SsPartState*>&		getPartSortList(){return sortList;}
	std::vector<SsPartAndAnime>&	getPartAnime(){ return	partAnime; }
	
	template<typename mytype> int	SsGetKeyValue( int time , SsAttribute* attr , mytype&  value );
	template<typename mytype> void	SsInterpolationValue( int time , const SsKeyframe* leftkey , const SsKeyframe* rightkey , mytype& v );
	void	SsInterpolationValue( int time , const SsKeyframe* leftkey , const SsKeyframe* rightkey , SsCellValue& v );
	void	SsInterpolationValue( int time , const SsKeyframe* leftkey , const SsKeyframe* rightkey , SsColorAnime& v );
	void	SsInterpolationValue( int time , const SsKeyframe* leftkey , const SsKeyframe* rightkey , SsVertexAnime& v );



};



#endif
