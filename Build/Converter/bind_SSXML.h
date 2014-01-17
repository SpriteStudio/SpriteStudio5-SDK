#ifndef __bind_SSXML__
#define __bind_SSXML__

#include "package_SpriteStudio.h"
#include "bind_AnimePack.h"


//ssLoaderにあるクラスのラッパークラスをPythonへの参照渡しを行う関係からラッピングする
//データのスクリプト側への受け渡しのため各ポインタはBind_SsProjectにあるm_projectのポインタ参照になる。
//SSXMLが解放されたタイミングでこれらの参照が消える仕組みとする。
//本来的には消えない方がpythonでの利用上適切ではあるが、解放タイミングのわかりやすさを重視してこのようにする。




class Bind_SsProject {
public:
	std::vector<Bind_SsAnimePack>	m_animepacklist;

	SsProject *m_project;
    Bind_SsProject();
	bool debug();

	void createAnimepack();

	///<プロジェクトに格納されたアニメーションのファイル数を取得する
	int	getAnimePackNum(){ 
		if ( m_project )
			return m_project->getAnimePackNum();
		return 0;
	}

	///<プロジェクトに格納されたセルマップのファイル数を取得する
	int	getCellMapNum(){ 
		if ( m_project )
			return m_project->getCellMapNum();
		return 0;
	}
	
	Bind_SsAnimePack const& AnimePackAt( int at )
	{
		return m_animepacklist[at];
	}

};


class SSXML
{
private:
	Bind_SsProject*	m_bind_project;
public:
	SSXML();
	bool	Load( const char* name );

	Bind_SsProject	const& GetPrj()
	{
		return 	*m_bind_project;
	}

};



#endif
