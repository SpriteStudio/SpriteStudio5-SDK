#ifndef __SSPLAYER_PLAYER__
#define __SSPLAYER_PLAYER__

#include "../../loader/ssloader.h"
#include "../helper/glHelper.h"





class SsPlayer;
class SsCelMapLinker;

///頂点変形キーの４頂点変形値
struct SsVertexAnime
{
	SsPoint2	offsets[4];	///< 各頂点の移動オフセット値
};

///パーツが使用するセルの情報
struct SsCellValue
{
	SsPlayer*		player;		///SsPlayerのポインタ　（SsCellMapManager使用のため）
	SsCell*			cell;		///参照しているセル
	SsCelMapLinker*	cellmapl;	///参照しているセルが所属しているSsCelMapLinker
	int				texture;	///テクスチャID 
	SsVector2		uvs[5];		///使用するUV座標

	SsCellValue() : player(0) , cell(0) , cellmapl(0) , texture(0) {}
};


///カラーブレンド使用時のブレンドタイプとカラー値
struct SsColorAnime
{
	SsColorBlendTarget::_enum	target;		//ブレンドの適用方法( 単色。全体 , 頂点単位 )
	SsBlendType::_enum			blendType;	//ブレンド種別 (mix　乗算　加算　減算）
	SsColorBlendValue			color;		//単色。全体の場合に使用されるカラー値
	SsColorBlendValue			colors[4];	//頂点単位の場合使用されるカラー値
};


///パーツの状態を保持するクラスです。
struct SsPartState
{
	int				index;				///パーツのインデックスと一対一になるID

	float			vertices[3 * 10];	///< 座標
	float			colors[4 * 4];		///< カラー (４頂点分）
	float			uvs[2 * 5];			///< UV		(４隅＋中央)
	float			matrix[4 * 4];		///< 行列


	SsPartState*	parent;			/// 親へのポインタ
	float* 			inheritRates;	///< 継承設定の参照先。inheritType がparentだと親のを見に行く。透過的に遡るのでルートから先に設定されている必要がある。

	SsVector3		position;		///< 位置。あくまで親パーツ基準のローカル座標
	SsVector3		rotation;		///< 回転角。degree
	SsVector2		scale;			///< スケール	
	float			alpha;			///< 不透明度 0~1
	int				prio;			///< 優先度
	bool			hFlip;			///< 水平反転
	bool			vFlip;			///< 垂直反転
	bool			hide;			///< 非表示にする
	SsVector2		pivotOffset;	///< 原点のオフセット。旧SSの原点は左上基準にしたこの値に相当する。0,0が中央+0.5,+0.5が右上になる。参照セルがある場合はセルの原点に＋する＝オフセット扱いになる。
	SsVector2		anchor;			///< アンカーポイント。親パーツのどの位置に引っ付けるか？0,0が中央+0.5,+0.5が右上になる。
	SsVector2		size;			///< 表示サイズ	
	bool			imageFlipH;		///　セル画像を水平反転するか
 	bool			imageFlipV;		///	 セル画像を垂直反転するか
	SsVector2		uvTranslate;	///< UV 平行移動
	float			uvRotation;		///< UV 回転
	SsVector2		uvScale;		///< UV スケール
	float			boundingRadius;	///< 当たり判定用の円の半径

	SsCellValue		cellValue;		///< セルアニメの値
	SsColorAnime	colorValue;		///< カラーアニメの値
	SsVertexAnime	vertexValue;	///< 頂点アニメの値

	bool			noCells;				/// セル参照が見つからない
	bool			is_color_blend;			/// カラーブレンドが使用される (描画コストが高いシェーダが使われるためフラグ化)
	bool			is_vertex_transform;	/// 頂点変形が使用される (描画コストが高いシェーダが使われるためフラグ化)

	SsBlendType::_enum	alphaBlendType;

	SsPartState(){init();}

	void	init()
	{
		memset( vertices , 0 , sizeof( vertices ) );
		//cell = 0;
		position = SsVector3( 0.0f , 0.0f, 0.0f );
		rotation = SsVector3( 0.0f , 0.0f , 0.0f );
		scale = SsVector2( 1.0f , 1.0f );

		anchor = SsVector2( 0 , 0 );
		size = SsVector2( 0 , 0 );

		alpha = 1.0f;
		prio = 0;
		hFlip = false;
		vFlip = false;
		hide = false;
		imageFlipH = false;
		imageFlipV = false;

		uvRotation = 0;
		uvTranslate = SsVector2( 0 , 0 );
		pivotOffset = SsVector2( 0 , 0 );
		uvScale = SsVector2( 1 , 1 );

		is_color_blend = false;
		is_vertex_transform = false;
		inheritRates = 0; 
	}

	bool	inherits_(SsAttributeKind::_enum kind) const {return inheritRates[(int)kind] != 0.f;}

};


//パーツとアニメを関連付ける
typedef std::pair<SsPart*,SsPartAnime*>	SsPartAndAnime;



class SsCelMapLinker
{
public:
	SsCellMap*	cellMap;
	tkTexture*	tex;

	std::map<SsString,SsCell*>	CellDic;

public:
	SsCelMapLinker()
		: cellMap(0) , tex(0)
	{}

	SsCelMapLinker(SsCellMap* cellmap ,SsString filePath )
	{

		cellMap = cellmap;
		size_t num = cellMap->cells.size();
		for ( size_t i = 0 ; i < num ; i++ )
		{
			CellDic[cellMap->cells[i]->name] = cellMap->cells[i];
		}

		tex = new tkTexture();

		SsString fullpath = filePath + cellmap->imagePath;

		if ( !tex->LoadImage( fullpath.c_str() ) )
		{
			delete tex;
			tex = 0;
		}

	}

	virtual ~SsCelMapLinker()
	{
		CellDic.clear();

		if ( tex )
			delete tex;
	}

	SsCell*	findCell( const SsString& name ){ return CellDic[name]; }
	
};


class	SsCellMapManager
{
private:
	//同名セルマップは上書き
	std::map<SsString,SsCelMapLinker*>		CellMapDic;
	typedef std::map<SsString,SsCelMapLinker*>::iterator CellMapDicItr;
	SsString	CellMapPath;

public:
	SsCellMapManager(){}
	virtual ~SsCellMapManager()
	{
		for ( CellMapDicItr itr = CellMapDic.begin() ; itr != CellMapDic.end() ; itr ++)
		{
			delete itr->second;
		}
		CellMapDic.clear();
	}

	void	clear();
	void	setCellMapPath(  const SsString& filepath );
	void	set(SsProject* proj);
	void	add(SsProject* proj);
	void	add(SsCellMap* cellmap);
	
	 SsCelMapLinker*	getCellMapLink( const SsString& name );
};


class SsPlayer
{
public:
	SsProject*		project;
	SsAnimation*	curAnime;
	SsModel*		curModel;
	SsAnimePack*	curAnimePack;

	std::vector<SsPartAndAnime>		part_anime;
	std::map<SsString,SsPartAnime*> PartAnimeDic;

	SsCellMapManager*		curCellMapManager;
	SsPartState*			partState;	///パーツの現在の状態が格納されています。
	std::list<SsPartState*> sortList;	///ソート状態


	float	nowPlatTime;
	int		curAnimeEndFrame;

	struct AnimationChangeMsg_{
		bool	change;
		int		animePack;
		int		anime;
	}AnimationChangeMsg;


private:
	void	updateState( int nowTime , SsPart* part , SsPartAnime* part_anime , SsPartState* state );
	void	updateMatrix(SsPart* part , SsPartAnime* anime , SsPartState* state);
	void	updateVertices(SsPart* part , SsPartAnime* anime , SsPartState* state);
	void	draw_part( SsPartState* state );


public:
	SsPlayer();
	virtual ~SsPlayer()
	{
		if ( curCellMapManager )
			delete curCellMapManager;

		if ( partState )
			delete [] partState;
	}

	static void	shaderInit();

	void	update();
	void	draw();
	void	setProject( SsProject* proj , int packIndex = 0, int animeIndex = 0) ;
	void	changeAnimation(int packIndex = 0, int animeIndex = 0);

	void	setPlayFrame( float time ) { nowPlatTime = time; }
	int		getAnimeEndFrame() { 
		if ( curAnime == 0 ) return 0;
		return curAnime->settings.frameCount; 
	}

	int		getAnimeFPS() { 
		if ( curAnime == 0 ) return 0;
		return curAnime->settings.fps; 
	}		

};



#endif
