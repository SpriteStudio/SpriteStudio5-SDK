#ifndef __SSLOADER_SSAE__
#define __SSLOADER_SSAE__

#include "sstypes.h"
#include "ssarchiver.h"
#include "ssattribute.h"


/// アニメーション再生設定情報です。
class SsAnimationSettings
{
public:
	int						fps;			//!< 再生FPS
	int						frameCount;		//!< フレーム数
	SsPartsSortMode::_enum	sortMode;		//!< パーツのソートモード
	SsPoint2		canvasSize;				//!< キャンバスサイズ(元基準枠)。ビューポートのサイズとイコールではない。
	SsVector2		pivot;					//!< キャンバスの原点。0,0 が中央。-0.5, +0.5 が左上

	SsAnimationSettings(){}
	virtual ~SsAnimationSettings(){}
public:

	///シリアライズのための宣言です。
	SSSERIALIZE_BLOCK
	{
		SSAR_DECLARE( fps );
		SSAR_DECLARE( frameCount );
		SSAR_DECLARE( canvasSize );
		SSAR_DECLARE( pivot );
		SSAR_DECLARE_ENUM(sortMode);
	}
};



//パーツ一つあたりの情報を保持するデータです。
class	SsPart
{
public:
	SsString				name;
	int						arrayIndex;		//!< ツリーを配列に展開した時のインデックス
	int						parentIndex;	//!< 親パーツのインデックス
	SsPartType::_enum		type;			//!< 種別
	SsBoundsType::_enum		boundsType;		//!< 当たり判定として使うか？使う場合はその形状。
	SsInheritType::_enum	inheritType;	//!< アトリビュート値の継承方法
	SsBlendType::_enum		alphaBlendType;	//!< αブレンドの演算式
	int						show;			//!< [編集用データ] パーツの表示・非常時
	int						locked;			//!< [編集用データ] パーツのロック状態

	float					inheritRates[(int)SsAttributeKind::num];	///< 親の値の継承率。SS4との互換性のため残されているが0 or 1

	//参照アニメーション名
	//ツールはともかくランタイムでは勿体ない使い方なので何らか考えること
	SsString        refAnimePack;   ///< 参照アニメ名
	SsString        refAnime;       ///< 参照アニメ名			

	SsString        refEffectName;	///< 割り当てたパーティクル名

	SsString        colorLabel;		///< カラーラベル

public:
	SsPart() : 
	  name("") , arrayIndex(0), parentIndex(0) , show(0) , locked(0)
	  {
			//memset( inheritRates , 0 , sizeof( float) * SsAttributeKind::num );
			for (int i = 0; i < (int)SsAttributeKind::num ; ++i)
				inheritRates[i] = 1.f;

			//継承率の初期値
			//継承のタグがない場合に、この値が適用される
			// 反転・非表示フラグは継承しない from 旧SS
			//SS5　では初期値は１
/*
			inheritRates[(int)SsAttributeKind::fliph] = 0.f;
			inheritRates[(int)SsAttributeKind::flipv] = 0.f;
			inheritRates[(int)SsAttributeKind::hide] = 0.f;
*/
			// イメージ反転も継承しない 2013.04.03 水 15:08
			inheritRates[(int)SsAttributeKind::imgfliph] = 0.f;
			inheritRates[(int)SsAttributeKind::imgflipv] = 0.f;
	  }
	virtual ~SsPart(){}


	///シリアライズのための宣言です。
	SSSERIALIZE_BLOCK
	{
		SSAR_DECLARE( name );
		SSAR_DECLARE( arrayIndex );
		SSAR_DECLARE( parentIndex );

		SSAR_DECLARE_ENUM( type );
		SSAR_DECLARE_ENUM( boundsType );
		SSAR_DECLARE_ENUM( inheritType );
		SSAR_DECLARE_ENUM( alphaBlendType );
		SSAR_DECLARE( show );
		SSAR_DECLARE( locked );
		SSAR_DECLARE( refAnimePack );
		SSAR_DECLARE( refAnime );
		SSAR_DECLARE( colorLabel );

		if ( type == SsPartType::effect )
		{
			SSAR_DECLARE( refEffectName ); 	
		}

		//継承率後に改良を実施
		if ( ar->getType() == EnumSsArchiver::in )
		{
			XMLElement* e = ar->getxml()->FirstChildElement( "ineheritRates" );
			if ( e )
			{
				XMLElement* ec = e->FirstChildElement();
				while(ec)
				{
					//継承設定の取得
					const char* tag = ec->Value();
					SsAttributeKind::_enum enumattr;

					__StringToEnum_( tag , enumattr );
					inheritRates[(int)enumattr] = (float)atof( ec->GetText() );
					ec = ec->NextSiblingElement();
				}
			}
		}

	}
};


//アニメーションを構成するパーツをリスト化したものです。
class SsModel
{
public:
	std::vector<SsPart*>	partList;	//!<格納されているパーツのリスト

public:
	SsModel(){}
	virtual~ SsModel()
	{
		for ( std::vector<SsPart*>::iterator itr = partList.begin() ; 
			itr != partList.end() ; itr ++ ) delete (*itr);
	}

	///シリアライズのための宣言です。
	SSSERIALIZE_BLOCK
	{
		SSAR_DECLARE_LIST( partList );
	}

};


typedef std::vector<SsAttribute*>	SsAttributeList;

class SsPartAnime
{
public:
	SsString					partName;
	SsAttributeList				attributes;

public:
	SsPartAnime(){}
	virtual ~SsPartAnime(){
		for ( SsAttributeList::iterator itr = attributes.begin() ; 
			itr != attributes.end() ; itr ++ ) delete (*itr);	
	}

	///シリアライズのための宣言です。
	SSSERIALIZE_BLOCK
	{
		SSAR_DECLARE( partName );
		SSAR_DECLARE_LISTEX( attributes , "attribute" );
	}
};


/// ラベル。ループ先などに指定する
class SsLabel
{
public:
	SsString	name;	///< 名前 [変数名変更禁止]
	int			time;	///< 設置された時間(フレーム) [変数名変更禁止]

	SSSERIALIZE_BLOCK
	{
		SSAR_DECLARE( name );
		SSAR_DECLARE( time );
	}

private:

};


class SsAnimation
{
public:
	SsString	name;							/// アニメーションの名称
	bool		overrideSettings;				/// このインスタンスが持つ設定を使いanimePack の設定を参照しない。FPS, frameCount は常に自身の設定を使う。
	SsAnimationSettings			settings;		/// 設定情報 
	std::vector<SsPartAnime*>	partAnimes;		///	パーツ毎のアニメーションキーフレームが格納されるリスト

	std::vector<SsLabel*>	labels;				/// アニメーションが持つラベルのリストです。
public:
	SsAnimation(){}
	virtual ~SsAnimation()
	{
		for ( std::vector<SsPartAnime*>::iterator itr = partAnimes.begin() ; 
			itr != partAnimes.end() ; itr ++ ) delete (*itr);	
		for ( std::vector<SsLabel*>::iterator itr = labels.begin() ; 
			itr != labels.end() ; itr ++ ) delete (*itr);	
	}

	///シリアライズのための宣言です。
	SSSERIALIZE_BLOCK
	{
		SSAR_DECLARE( name );
		SSAR_STRUCT_DECLARE( settings );
		SSAR_DECLARE_LISTEX( labels , "value" );
		SSAR_DECLARE_LISTEX( partAnimes , "partAnime" );
	}
};

/**
*@class SsAnimePack
*@brief パーツを組み合わせた構造とその構造を使用するアニメーションを格納するデータです。
パーツの組み合わせ構造をSsModel、Modelを使用するアニメデータをSsAnimationで定義しています。
*/
class SsAnimePack
{
public:
	SsString					version;
	SsAnimationSettings			settings;		//!< 設定情報 
	SsString					name;			//!< アニメーションパック名称
	SsModel						Model;			//!< パーツ情報の格納先
	std::vector<SsString>		cellmapNames;	//!< 使用されているセルマップの名称		
	std::vector<SsAnimation*>	animeList;		//!< 格納されている子アニメーションのリスト

	SsAnimePack(){}
	virtual ~SsAnimePack()
	{
		for ( std::vector<SsAnimation*>::iterator itr = animeList.begin() ; 
			itr != animeList.end() ; itr ++ ) delete (*itr);		
	}

	///シリアライズのための宣言です。
	SSSERIALIZE_BLOCK
	{
		SSAR_DECLARE_ATTRIBUTE(version);
		SSAR_STRUCT_DECLARE( settings );
		SSAR_DECLARE( name );
		SSAR_STRUCT_DECLARE( Model );
		SSAR_DECLARE( cellmapNames );
		SSAR_DECLARE_LISTEX( animeList , "anime" );
	}

	//アニメーション名からアニメーションを取得する
	SsAnimation*	findAnimation(SsString& name);

};

/*
* @class ssloader_ssae
* @brief ssaeファイルをロードするためのクラスです。
*/
class	ssloader_ssae
{
public:
	ssloader_ssae(){}
	virtual ~ssloader_ssae(){}

	///ssaeファイル名を指定しロードが成功したらそのSsAnimePackのポインタを返します。
	static SsAnimePack*	Load(const std::string& filename );

};






#endif
