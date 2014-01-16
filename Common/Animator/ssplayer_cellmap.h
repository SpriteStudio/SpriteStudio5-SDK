#ifndef __SSPLAYER_CELLMAP__
#define __SSPLAYER_CELLMAP__



class SsAnimeDecoder;
class SsCelMapLinker;


///パーツが使用するセルの情報
struct SsCellValue
{
	//SsAnimeDecoder*	player;		///SsPlayerのポインタ　（SsCellMapManager使用のため）

	SsCell*			cell;		///参照しているセル
	SsCelMapLinker*	cellmapl;	///参照しているセルが所属しているSsCelMapLinker
	ISSTexture*		texture;	///テクスチャ
	SsVector2		uvs[5];		///使用するUV座標


	SsCellValue() :  
	cell(0) , cellmapl(0) , texture(0) 
	{}
};

class SsCelMapLinker
{
public:
	SsCellMap*			cellMap;
	ISSTexture*	tex;

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

		tex = SSTextureFactory::create();
		SsString fullpath = filePath + cellmap->imagePath;

		if ( !tex->Load( fullpath.c_str() ) )
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


class	SsCellMapList
{
private:
	//同名セルマップは上書き
	std::map<SsString,SsCelMapLinker*>		CellMapDic;
	typedef std::map<SsString,SsCelMapLinker*>::iterator CellMapDicItr;
	SsString	CellMapPath;

public:
	SsCellMapList(){}
	virtual ~SsCellMapList()
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

void calcUvs( SsCellValue* cellv );

#endif
