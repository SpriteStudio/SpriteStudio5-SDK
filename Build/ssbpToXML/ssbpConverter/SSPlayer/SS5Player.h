/** 
*  SS5Player.h
*/


/************************************************************

- Quick start
 
  #include "SS5Player.h"

  
  // SS5プレイヤーの宣言
  ss::Player *ssplayer;
  ss::ResourceManager *resman;


  //リソースマネージャの作成
  resman = ss::ResourceManager::getInstance();
  //プレイヤーの作成
  ssplayer = ss::Player::create();

  //アニメデータをリソースに追加
  //それぞれのプラットフォームに合わせたパスへ変更してください。
  resman->addData("character_template_comipo\\character_template1.ssbp");
  //プレイヤーにリソースを割り当て
  ssplayer->setData("character_template1");					// ssbpファイル名（拡張子不要）
  //再生するモーションを設定
  ssplayer->play("character_template_3head/stance");		// アニメーション名を指定(ssae名/アニメーション名も可能、詳しくは後述)


  //表示位置を設定
  ssplayer->setPosition(1280/2, 720);
  //スケール設定
  ssplayer->setScale(0.5f, 0.5f);
  //回転を設定
  ssplayer->setRotation(0.0f, 0.0f, 0.0f);
  //透明度を設定
  ssplayer->setAlpha(255);
  //反転を設定
  ssplayer->setFlip(false, false);


  //メインループで呼び出してください。
  ssplayer->update(dt);					//プレイヤーの更新
  ssplayer->draw();						//プレイヤーの描画


  終了処理で resman、player を delete してください。
  //テクスチャの解放
  resman->releseTexture("character_template1");
  //SS5Playerの削除
  delete (ssplayer);
  delete (resman);

*************************************************************/


#ifndef SS5Player_h
#define SS5Player_h

#include "SS5PlayerData.h"
#include "SS5PlayerTypes.h"
#include "SS5PlayerPlatform.h"
#include <map>
#include <vector>
#include <string>
#include <stdarg.h>
#include <assert.h>

#pragma warning(disable : 4996)

//出力用テクスチャーデータ
#define TEXDATA_MAX (512)
struct TexureData
{
	int id;
	std::string name;
};

//出力用パーツデータ
#define PARTDATA_MAX (512)
struct RECT
{
	int x;
	int y;
	int w;
	int h;
};

struct PARTDATA
{
	std::string name;				//
	int flags;						/// このフレームで更新が行われるステータスのフラグ
	int cellIndex;					/// パーツに割り当てられたセルの番号
	float x;						/// SS5アトリビュート：X座標
	float y;						/// SS5アトリビュート：Y座標
	float z;						/// SS5アトリビュート：Z座標
	float anchorX;					/// 原点Xオフセット＋セルに設定された原点オフセットX
	float anchorY;					/// 原点Yオフセット＋セルに設定された原点オフセットY
	float rotationX;				/// X回転（親子関係計算済）
	float rotationY;				/// Y回転（親子関係計算済）
	float rotationZ;				/// Z回転（親子関係計算済）
	float scaleX;					/// Xスケール（親子関係計算済）
	float scaleY;					/// Yスケール（親子関係計算済）
	int opacity;					/// 不透明度（0～255）（親子関係計算済）
	float size_X;					/// SS5アトリビュート：Xサイズ
	float size_Y;					/// SS5アトリビュート：Xサイズ
	float uv_move_X;				/// SS5アトリビュート：UV X移動
	float uv_move_Y;				/// SS5アトリビュート：UV Y移動
	float uv_rotation;				/// SS5アトリビュート：UV 回転
	float uv_scale_X;				/// SS5アトリビュート：UV Xスケール
	float uv_scale_Y;				/// SS5アトリビュート：UV Yスケール
	float boundingRadius;			/// SS5アトリビュート：当たり半径
	int colorBlendFunc;				/// SS5アトリビュート：カラーブレンドのブレンド方法
	int colorBlendType;				/// SS5アトリビュート：カラーブレンドの単色か頂点カラーか。
	bool flipX;						/// 横反転（親子関係計算済）
	bool flipY;						/// 縦反転（親子関係計算済）
	bool isVisibled;				/// 非表示（親子関係計算済）
	float instancerotationX;		/// インスタンスパーツに設定されたX回転
	float instancerotationY;		/// インスタンスパーツに設定されたY回転
	float instancerotationZ;		/// インスタンスパーツに設定されたZ回転
	ss::SSV3F_C4B_T2F_Quad quad;		/// 頂点データ、座標、カラー値、UVが含まれる（頂点変形、サイズXY、UV移動XY、UVスケール、UV回転、反転が反映済）
	long texture;					/// セルに対応したテクスチャ番号（ゲーム側で管理している番号を設定する）
	RECT rect;						/// セルに対応したテクスチャ内の表示領域（開始座標、幅高さ）
	int blendfunc;					/// パーツに設定されたブレンド方法
	float mat[16];					/// パーツの位置を算出するためのマトリクス（親子関係計算済）
};

namespace ss
{
class CustomSprite;
class CellCache;
class CellRef;
class AnimeCache;
class AnimeRef;
class ResourceSet;
struct ProjectData;
class SSSize;

/**
* 定数
*/
#define __PI__	(3.14159265358979323846f)
#define __2PI__	(__PI__ * 2)
#define RadianToDegree(Radian) ((float)( Radian * __2PI__ )/ 360.0f )
#define DegreeToRadian(Degree) ((float)( Degree * 360.0f) / __2PI__)


#define SS_SAFE_DELETE(p)            do { if(p) { delete (p); (p) = 0; } } while(0)
#define SS_SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = 0; } } while(0)
#define SS_SAFE_FREE(p)                do { if(p) { free(p); (p) = 0; } } while(0)
#define SS_SAFE_RELEASE(p)            do { if(p) { (p)->release(); } } while(0)
#define SS_SAFE_RELEASE_NULL(p)        do { if(p) { (p)->release(); (p) = 0; } } while(0)
#define SS_SAFE_RETAIN(p)            do { if(p) { (p)->retain(); } } while(0)
#define SS_BREAK_IF(cond)            if(cond) break

#ifdef _DEBUG
	#define SSLOG(...)       do {} while (0)
	#define SS_ASSERT(cond)    assert(cond)
	#define SS_ASSERT2(cond, msg) SS_ASSERT(cond)
	#define SSLOGERROR(format,...)  do {} while (0)
#else
	#define SSLOG(...)       do {} while (0)
	#define SS_ASSERT(cond)
	#define SS_ASSERT2(cond, msg) ((void)(cond))
	#define SSLOGERROR(format,...)  do {} while (0)
#endif


#define DOT (10.0f)					/// 固定少数の定数 10=1ドット
#define PART_VISIBLE_MAX (512)		/// １アニメの最大パーツ数


/**
* 座標クラス
*/
class SSPoint
{
public:
	float x;
	float y;

public:
	SSPoint();
	SSPoint(float x, float y);
	SSPoint(const SSPoint& other);
	SSPoint(const SSSize& size);
	SSPoint& operator= (const SSPoint& other);
	SSPoint& operator= (const SSSize& size);
	SSPoint operator+(const SSPoint& right) const;
	SSPoint operator-(const SSPoint& right) const;
	SSPoint operator-() const;
	SSPoint operator*(float a) const;
	SSPoint operator/(float a) const;
	void setPoint(float x, float y);
	bool equals(const SSPoint& target) const;
	bool fuzzyEquals(const SSPoint& target, float variance) const;
	inline float getLength() const {
		return sqrtf(x*x + y*y);
	};
	inline float getLengthSq() const {
		return dot(*this); //x*x + y*y;
	};
	inline float getDistanceSq(const SSPoint& other) const {
		return (*this - other).getLengthSq();
	};
	inline float getDistance(const SSPoint& other) const {
		return (*this - other).getLength();
	};
	inline float getAngle() const {
		return atan2f(y, x);
	};
	float getAngle(const SSPoint& other) const;
	inline float dot(const SSPoint& other) const {
		return x*other.x + y*other.y;
	};
	inline float cross(const SSPoint& other) const {
		return x*other.y - y*other.x;
	};
	inline SSPoint getPerp() const {
		return SSPoint(-y, x);
	};
	inline SSPoint getRPerp() const {
		return SSPoint(y, -x);
	};
	inline SSPoint project(const SSPoint& other) const {
		return other * (dot(other) / other.dot(other));
	};
	inline SSPoint rotate(const SSPoint& other) const {
		return SSPoint(x*other.x - y*other.y, x*other.y + y*other.x);
	};
	inline SSPoint unrotate(const SSPoint& other) const {
		return SSPoint(x*other.x + y*other.y, y*other.x - x*other.y);
	};
	inline SSPoint normalize() const {
		float length = getLength();
		if (length == 0.) return SSPoint(1.f, 0);
		return *this / getLength();
	};
	inline SSPoint lerp(const SSPoint& other, float alpha) const {
		return *this * (1.f - alpha) + other * alpha;
	};
	SSPoint rotateByAngle(const SSPoint& pivot, float angle) const;

	static inline SSPoint forAngle(const float a)
	{
		return SSPoint(cosf(a), sinf(a));
	}
};

/**
* サイズクラス
*/
class SSSize
{
public:
	float width;
	float height;

public:
	SSSize();
	SSSize(float width, float height);
	SSSize(const SSSize& other);
	SSSize(const SSPoint& point);
	SSSize& operator= (const SSSize& other);
	SSSize& operator= (const SSPoint& point);
	SSSize operator+(const SSSize& right) const;
	SSSize operator-(const SSSize& right) const;
	SSSize operator*(float a) const;
	SSSize operator/(float a) const;
	void setSize(float width, float height);
	bool equals(const SSSize& target) const;
};

/**
* 矩形クラス
*/
class SSRect
{
public:
	SSPoint origin;
	SSSize  size;

public:
	SSRect();
	SSRect(float x, float y, float width, float height);
	SSRect(const SSRect& other);
	SSRect& operator= (const SSRect& other);
	void setRect(float x, float y, float width, float height);
	float getMinX() const; /// return the leftmost x-value of current rect
	float getMidX() const; /// return the midpoint x-value of current rect
	float getMaxX() const; /// return the rightmost x-value of current rect
	float getMinY() const; /// return the bottommost y-value of current rect
	float getMidY() const; /// return the midpoint y-value of current rect
	float getMaxY() const; /// return the topmost y-value of current rect
	bool equals(const SSRect& rect) const;
	bool containsPoint(const SSPoint& point) const;
	bool intersectsRect(const SSRect& rect) const;
};

// printf 形式のフォーマット
static std::string Format(const char* format, ...){

	static std::vector<char> tmp(1000);

	va_list args, source;
	va_start(args, format);
	source = args;

	while (1)
	{
		args = source;
		//Windows
		if (_vsnprintf(&tmp[0], tmp.size(), format, args) == -1)
		{
			tmp.resize(tmp.size() * 2);
		}
		else
		{
			break;
		}
	}
	tmp.push_back('\0');
	std::string ret = &(tmp[0]);
	va_end(args);
	return ret;
}


/**
* ToPointer
*/
class ToPointer
{
public:
	explicit ToPointer(const void* base)
		: _base(static_cast<const char*>(base)) {}

	const void* operator()(ss_offset offset) const
	{
		return (_base + offset);
	}

private:
	const char*	_base;
};


/**
* DataArrayReader
*/
class DataArrayReader
{
public:
	DataArrayReader(const ss_u16* dataPtr)
		: _dataPtr(dataPtr)
	{}

	ss_u16 readU16() { return *_dataPtr++; }
	ss_s16 readS16() { return static_cast<ss_s16>(*_dataPtr++); }

	unsigned int readU32()
	{
		unsigned int l = readU16();
		unsigned int u = readU16();
		return static_cast<unsigned int>((u << 16) | l);
	}

	int readS32()
	{
		return static_cast<int>(readU32());
	}

	float readFloat()
	{
		union {
			float			f;
			unsigned int	i;
		} c;
		c.i = readU32();
		return c.f;
	}

	void readColor(SSColor4B& color)
	{
		unsigned int raw = readU32();
		color.a = static_cast<unsigned char>(raw >> 24);
		color.r = static_cast<unsigned char>(raw >> 16);
		color.g = static_cast<unsigned char>(raw >> 8);
		color.b = static_cast<unsigned char>(raw);
	}

	ss_offset readOffset()
	{
		return static_cast<ss_offset>(readS32());
	}

private:
	const ss_u16*	_dataPtr;
};

/**
* ResourceSet
*/
struct ResourceSet
{
	const ProjectData* data;
	bool isDataAutoRelease;
	CellCache* cellCache;
	AnimeCache* animeCache;

	virtual ~ResourceSet()
	{
		if (isDataAutoRelease)
		{
			delete data;
			data = NULL;
		}
		if (animeCache)
		{
			delete animeCache;
			animeCache = NULL;
		}
		if (cellCache)
		{
			delete cellCache;
			cellCache = NULL;
		}
	}
};
/**
* CellRef
*/
struct CellRef
{
	const Cell* cell;
	long texture;
	SSRect rect;
};


/**
* CellCache
*/
class CellCache
{
public:
	virtual ~CellCache()
	{
	}

	static CellCache* create(const ProjectData* data, const std::string& imageBaseDir)
	{
		CellCache* obj = new CellCache();
		if (obj)
		{
			obj->init(data, imageBaseDir);
		}
		return obj;
	}

	CellRef* getReference(int index)
	{
		if (index < 0 || index >= (int)_refs.size())
		{
			SSLOGERROR("Index out of range > %d", index);
			SS_ASSERT(0);
		}
		CellRef* ref = _refs.at(index);
		return ref;
	}

	//指定した名前のセルの参照テクスチャを変更する
	bool setCellRefTexture(const ProjectData* data, const char* cellName, long texture)
	{
		bool rc = false;

		ToPointer ptr(data);
		const Cell* cells = static_cast<const Cell*>(ptr(data->cells));

		//名前からインデックスの取得
		int cellindex = -1;
		for (int i = 0; i < data->numCells; i++)
		{
			const Cell* cell = &cells[i];
			const CellMap* cellMap = static_cast<const CellMap*>(ptr(cell->cellMap));
			const char* name = static_cast<const char*>(ptr(cellMap->name));
			if (strcmp(cellName, name) == 0)
			{
				CellRef* ref = getReference(i);
				ref->texture = texture;
				rc = true;
			}
		}

		return(rc);
	}

	//指定したデータのテクスチャを破棄する
	bool releseTexture(const ProjectData* data)
	{
		bool rc = false;

		ToPointer ptr(data);
		const Cell* cells = static_cast<const Cell*>(ptr(data->cells));
		for (int i = 0; i < data->numCells; i++)
		{
			const Cell* cell = &cells[i];
			const CellMap* cellMap = static_cast<const CellMap*>(ptr(cell->cellMap));
			{
				CellRef* ref = _refs.at(i);
				if (ref->texture != -1)
				{
//					SSTextureRelese(ref->texture);
					ref->texture = -1;
					rc = true;
				}
			}
		}
		return(rc);
	}

protected:
	void init(const ProjectData* data, const std::string& imageBaseDir)
	{

		SS_ASSERT2(data != NULL, "Invalid data");

		_textures.clear();
		_refs.clear();

		ToPointer ptr(data);
		const Cell* cells = static_cast<const Cell*>(ptr(data->cells));

		for (int i = 0; i < data->numCells; i++)
		{
			const Cell* cell = &cells[i];
			const CellMap* cellMap = static_cast<const CellMap*>(ptr(cell->cellMap));

			if (cellMap->index >= (int)_textures.size())
			{
				const char* imagePath = static_cast<const char*>(ptr(cellMap->imagePath));
				addTexture(imagePath, imageBaseDir);
			}

			//セル情報だけ入れておく
			//テクスチャの読み込みはゲーム側に任せる
			CellRef* ref = new CellRef();
			ref->cell = cell;
			ref->texture = _textures.at(cellMap->index);
			ref->rect = SSRect(cell->x, cell->y, cell->width, cell->height);
			_refs.push_back(ref);
		}

	}

	void addTexture(const std::string& imagePath, const std::string& imageBaseDir)
	{
		std::string path = "";
		path = imagePath;
		/*
		if (isAbsolutePath(imagePath))
		{
			// 絶対パスのときはそのまま扱う
			path = imagePath;
		}
		else
		{
			// 相対パスのときはimageBaseDirを付与する
			path.append(imageBaseDir);
			size_t pathLen = path.length();
			if (pathLen && path.at(pathLen - 1) != '/' && path.at(pathLen - 1) != '\\')
			{
				path.append("/");
			}
			path.append(imagePath);
		}
*/
extern long SSTextureLoad(const char* pszFileName );


		//テクスチャの読み込み
		long tex = SSTextureLoad(path.c_str());
		SSLOG("load: %s", path.c_str());
		_textures.push_back(tex);

	}

protected:
	std::vector<long>					_textures;
	std::vector<CellRef*>				_refs;
};


/**
* AnimeRef
*/
struct AnimeRef
{
	std::string				packName;
	std::string				animeName;
	const AnimationData*	animationData;
	const AnimePackData*	animePackData;
};


/**
* AnimeCache
*/
class AnimeCache
{
public:
	static AnimeCache* create(const ProjectData* data)
	{
		AnimeCache* obj = new AnimeCache();
		if (obj)
		{
			obj->init(data);
		}
		return obj;
	}

	/**
	* packNameとanimeNameを指定してAnimeRefを得る
	*/
	AnimeRef* getReference(const std::string& packName, const std::string& animeName)
	{
		std::string key = toPackAnimeKey(packName, animeName);
		AnimeRef* ref = _dic.at(key);
		return ref;
	}

	/**
	* animeNameのみ指定してAnimeRefを得る
	*/
	AnimeRef* getReference(const std::string& animeName)
	{
		AnimeRef* ref = _dic.at(animeName);
		return ref;
	}

	std::map<std::string, AnimeRef*> getdicReference()
	{
		return _dic;
	}

	void dump()
	{
		std::map<std::string, AnimeRef*>::iterator it = _dic.begin();
		while (it != _dic.end())
		{
			SSLOG("%s", (*it).second);
			++it;
		}
	}

protected:
	void init(const ProjectData* data)
	{
		SS_ASSERT2(data != NULL, "Invalid data");

		ToPointer ptr(data);
		const AnimePackData* animePacks = static_cast<const AnimePackData*>(ptr(data->animePacks));

		for (int packIndex = 0; packIndex < data->numAnimePacks; packIndex++)
		{
			const AnimePackData* pack = &animePacks[packIndex];
			const AnimationData* animations = static_cast<const AnimationData*>(ptr(pack->animations));
			const char* packName = static_cast<const char*>(ptr(pack->name));

			for (int animeIndex = 0; animeIndex < pack->numAnimations; animeIndex++)
			{
				const AnimationData* anime = &animations[animeIndex];
				const char* animeName = static_cast<const char*>(ptr(anime->name));

				AnimeRef* ref = new AnimeRef();
				ref->packName = packName;
				ref->animeName = animeName;
				ref->animationData = anime;
				ref->animePackData = pack;

				// packName + animeNameでの登録
				std::string key = toPackAnimeKey(packName, animeName);
				SSLOG("anime key: %s", key.c_str());
				_dic.insert(std::map<std::string, AnimeRef*>::value_type(key, ref));

				// animeNameのみでの登録
				_dic.insert(std::map<std::string, AnimeRef*>::value_type(animeName, ref));

			}
		}
	}

	static std::string toPackAnimeKey(const std::string& packName, const std::string& animeName)
	{
		return Format("%s/%s", packName.c_str(), animeName.c_str());
	}

//protected:
public:
	std::map<std::string, AnimeRef*>	_dic;
};


/**
 * ResourceManager
 */
//class ResourceManager : public Ref
class ResourceManager
{
public:
	static const std::string s_null;

	/**
	 * デフォルトインスタンスを取得します.
	 *
	 * @return デフォルトのResourceManagerインスタンス
	 */
	static ResourceManager* getInstance();

	/**
	 * ssbpファイルを読み込み管理対象とします.
	 * dataKeyはssbpのファイル名（拡張子なし）になります.
	 *
	 * @param  ssbpFilepath  ssbpファイルのパス
	 * @param  imageBaseDir  画像ファイルの読み込み元ルートパス. 省略時はssbpのある場所をルートとします.
	 * @return dataKey
	 */
	std::string addData(const std::string& ssbpFilepath, const std::string& imageBaseDir = s_null);

	/**
	 * ssbpファイルを読み込み管理対象とします.
	 *
	 * @param  dataKey       dataKeyの指定
	 * @param  ssbpFilepath  ssbpファイルのパス
	 * @param  imageBaseDir  画像ファイルの読み込み元ルートパス. 省略時はssbpのある場所をルートとします.
	 * @return dataKey
	 */
	std::string addDataWithKey(const std::string& dataKey, const std::string& ssbpFilepath, const std::string& imageBaseDir = s_null);

	/**
	 * 指定されたssbpデータを管理対象とします.
	 *
	 * @param  dataKey       dataKeyの指定
	 * @param  data          ssbpデータ
	 * @param  imageBaseDir  画像ファイルの読み込み元ルートパス. 省略時はssbpのある場所をルートとします.
	 * @return dataKey
	 */
	std::string addData(const std::string& dataKey, const ProjectData* data, const std::string& imageBaseDir = s_null);
	
	/**
	 * 指定データを解放します.
	 *
	 * @param  dataKey
	 */
	void removeData(const std::string& dataKey);

	/**
	 * 全てのデータを解放します.
	 */
	void removeAllData();

	/**
	* 名前に対応するデータ取得します.
	*/
	ResourceSet* getData(const std::string& dataKey);

	/**
	* 指定したセルのテクスチャを変更します.
	* @param  dataName       ssbp名（拡張子を除くファイル名）
	* @param  callName       ssce名（拡張子を除くファイル名）
	* @param  texture        変更後のテクスチャハンドル
	* @return 変更を行ったか
	*/
	bool changeTexture(char* dataName, char* callName, long texture);

	/**
	* 指定したデータのテクスチャを破棄します。
	* @param  dataName       ssbp名（拡張子を除くファイル名）
	* @return 成功失敗
	*/
	bool releseTexture(char* dataName);

	/**
	 * 新たなResourceManagerインスタンスを構築します.
	 *
	 * @return ResourceManagerインスタンス
	 */
	static ResourceManager* create();

public:
	ResourceManager(void);
	virtual ~ResourceManager();

protected:
	std::map<std::string, ResourceSet*>	_dataDic;
};



/**
 * UserData
 */
struct UserData
{
	enum {
		FLAG_INTEGER	= 1 << 0,
		FLAG_RECT		= 1 << 1,
		FLAG_POINT		= 1 << 2,
		FLAG_STRING		= 1 << 3
	};

	const char*	partName;		/// Part name
	int			frameNo;		/// Frame no

	int			flags;			/// Flags of valid data
	int			integer;		/// Integer
	int			rect[4];		/// Rectangle Left, Top, Right, Bottom
	int			point[2];		/// Position X, Y
	const char*	str;			/// String (zero terminated)
	int			strSize;		/// String size (byte count)
};


/**
* LabelData
*/
struct LabelData
{
	std::string	str;			/// String (zero terminated)
	int			strSize;		/// String size (byte count)
	int			frameNo;		/// Frame no
};


/**
* State
  パーツの情報を格納します。Stateの内容をもとに描画処理を作成してください。
*/
struct State
{
	int flags;						/// このフレームで更新が行われるステータスのフラグ
	int cellIndex;					/// パーツに割り当てられたセルの番号
	float x;						/// SS5アトリビュート：X座標
	float y;						/// SS5アトリビュート：Y座標
	float z;						/// SS5アトリビュート：Z座標
	float anchorX;					/// 原点Xオフセット＋セルに設定された原点オフセットX
	float anchorY;					/// 原点Yオフセット＋セルに設定された原点オフセットY
	float rotationX;				/// X回転（親子関係計算済）
	float rotationY;				/// Y回転（親子関係計算済）
	float rotationZ;				/// Z回転（親子関係計算済）
	float scaleX;					/// Xスケール（親子関係計算済）
	float scaleY;					/// Yスケール（親子関係計算済）
	int opacity;					/// 不透明度（0～255）（親子関係計算済）
	float size_X;					/// SS5アトリビュート：Xサイズ
	float size_Y;					/// SS5アトリビュート：Xサイズ
	float uv_move_X;				/// SS5アトリビュート：UV X移動
	float uv_move_Y;				/// SS5アトリビュート：UV Y移動
	float uv_rotation;				/// SS5アトリビュート：UV 回転
	float uv_scale_X;				/// SS5アトリビュート：UV Xスケール
	float uv_scale_Y;				/// SS5アトリビュート：UV Yスケール
	float boundingRadius;			/// SS5アトリビュート：当たり半径
	int colorBlendFunc;				/// SS5アトリビュート：カラーブレンドのブレンド方法
	int colorBlendType;				/// SS5アトリビュート：カラーブレンドの単色か頂点カラーか。
	bool flipX;						/// 横反転（親子関係計算済）
	bool flipY;						/// 縦反転（親子関係計算済）
	bool isVisibled;				/// 非表示（親子関係計算済）
	float instancerotationX;		/// インスタンスパーツに設定されたX回転
	float instancerotationY;		/// インスタンスパーツに設定されたY回転
	float instancerotationZ;		/// インスタンスパーツに設定されたZ回転
	SSV3F_C4B_T2F_Quad quad;		/// 頂点データ、座標、カラー値、UVが含まれる（頂点変形、サイズXY、UV移動XY、UVスケール、UV回転、反転が反映済）
	long texture;					/// セルに対応したテクスチャ番号（ゲーム側で管理している番号を設定する）
	SSRect rect;					/// セルに対応したテクスチャ内の表示領域（開始座標、幅高さ）
	int blendfunc;					/// パーツに設定されたブレンド方法
	float mat[16];					/// パーツの位置を算出するためのマトリクス（親子関係計算済）

	void init()
	{
		flags = 0;
		cellIndex = 0;
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		anchorX = 0.0f;
		anchorY = 0.0f;
		rotationX = 0.0f;
		rotationY = 0.0f;
		rotationZ = 0.0f;
		scaleX = 1.0f;
		scaleY = 1.0f;
		opacity = 255;
		size_X = 1.0f;
		size_Y = 1.0f;
		uv_move_X = 0.0f;
		uv_move_Y = 0.0f;
		uv_rotation = 0.0f;
		uv_scale_X = 1.0f;
		uv_scale_Y = 1.0f;
		boundingRadius = 0.0f;
		colorBlendFunc = 0;
		colorBlendType = 0;
		flipX = false;
		flipY = false;
		isVisibled = false;
		instancerotationX = 0.0f;
		instancerotationY = 0.0f;
		instancerotationZ = 0.0f;
		memset(&quad, 0, sizeof(quad) );
		texture = 0;
		rect.size.height = 0;
		rect.size.width = 0;
		rect.origin.x = 0;
		rect.origin.y = 0;
		blendfunc = 0;
		memset(&mat, 0, sizeof(mat));
	}

	State() { init(); }
};

/**
* ResluteState
* ゲーム側に返すパーツステータス。
* 必要に応じてカスタマイズしてください。
*/
struct ResluteState
{
	float	x;	//座標X
	float	y;	//座標Y
};

/**
* 再生するフレームに含まれるパーツデータのフラグ
*/
enum {
	PART_FLAG_INVISIBLE = 1 << 0,				/// 非表示
	PART_FLAG_FLIP_H = 1 << 1,					/// 横反転
	PART_FLAG_FLIP_V = 1 << 2,					/// 縦反転

	// optional parameter flags
	PART_FLAG_CELL_INDEX = 1 << 3,				/// セル番号
	PART_FLAG_POSITION_X = 1 << 4,				/// X座標
	PART_FLAG_POSITION_Y = 1 << 5,				/// Y座標
	PART_FLAG_POSITION_Z = 1 << 6,				/// Z座標
	PART_FLAG_ANCHOR_X = 1 << 7,				/// 原点オフセットX
	PART_FLAG_ANCHOR_Y = 1 << 8,				/// 原点オフセットY
	PART_FLAG_ROTATIONX = 1 << 9,				/// X回転
	PART_FLAG_ROTATIONY = 1 << 10,				/// Y回転
	PART_FLAG_ROTATIONZ = 1 << 11,				/// Z回転
	PART_FLAG_SCALE_X = 1 << 12,				/// スケールX
	PART_FLAG_SCALE_Y = 1 << 13,				/// スケールY
	PART_FLAG_OPACITY = 1 << 14,				/// 不透明度
	PART_FLAG_COLOR_BLEND = 1 << 15,			/// カラーブレンド
	PART_FLAG_VERTEX_TRANSFORM = 1 << 16,		/// 頂点変形

	PART_FLAG_SIZE_X = 1 << 17,					/// サイズX
	PART_FLAG_SIZE_Y = 1 << 18,					/// サイズY

	PART_FLAG_U_MOVE = 1 << 19,					/// UV移動X
	PART_FLAG_V_MOVE = 1 << 20,					/// UV移動Y
	PART_FLAG_UV_ROTATION = 1 << 21,			/// UV回転
	PART_FLAG_U_SCALE = 1 << 22,				/// UVスケールX
	PART_FLAG_V_SCALE = 1 << 23,				/// UVスケールY
	PART_FLAG_BOUNDINGRADIUS = 1 << 24,			/// 当たり半径

	PART_FLAG_INSTANCE_KEYFRAME = 1 << 25,		/// インスタンス
	PART_FLAG_INSTANCE_START = 1 << 26,			/// インスタンス：開始フレーム
	PART_FLAG_INSTANCE_END = 1 << 27,			/// インスタンス：終了フレーム
	PART_FLAG_INSTANCE_SPEED = 1 << 28,			/// インスタンス：再生速度
	PART_FLAG_INSTANCE_LOOP = 1 << 29,			/// インスタンス：ループ回数
	PART_FLAG_INSTANCE_LOOP_FLG = 1 << 30,		/// インスタンス：ループ設定

	NUM_PART_FLAGS
};

/**
* 頂点変形フラグ
*/
enum {
	VERTEX_FLAG_LT = 1 << 0,
	VERTEX_FLAG_RT = 1 << 1,
	VERTEX_FLAG_LB = 1 << 2,
	VERTEX_FLAG_RB = 1 << 3,
	VERTEX_FLAG_ONE = 1 << 4	// color blend only
};

/**
* インスタンスループ設定フラグ
*/
enum {
	INSTANCE_LOOP_FLAG_INFINITY = 1 << 0,		//
	INSTANCE_LOOP_FLAG_REVERSE = 1 << 1,
	INSTANCE_LOOP_FLAG_PINGPONG = 1 << 2,
	INSTANCE_LOOP_FLAG_INDEPENDENT = 1 << 3,
};

/**
* Animation Part Type
*/
enum
{
	PARTTYPE_INVALID = -1,
	PARTTYPE_NULL,			/// null。領域を持たずSRT情報のみ。ただし円形の当たり判定は設定可能。
	PARTTYPE_NORMAL,		/// 通常パーツ。領域を持つ。画像は無くてもいい。
	PARTTYPE_TEXT,			/// テキスト(予約　未実装）
	PARTTYPE_INSTANCE,		/// インスタンス。他アニメ、パーツへの参照。シーン編集モードの代替になるもの
	PARTTYPE_NUM
};

/*
* 当たり判定の種類
*/
enum
{
	INVALID = -1,
	NONE,			///< 当たり判定として使わない。
	QUAD,			///< 自在に変形する四辺形。頂点変形など適用後の４角を結んだ領域。最も重い。
	AABB,			///< 回転しない全体を囲む矩形で交差判定
	CIRCLE,			///< 真円の半径で距離により判定する
	CIRCLE_SMIN,	///< 真円の半径で距離により判定する (スケールはx,yの最小値をとる）
	CIRCLE_SMAX,	///< 真円の半径で距離により判定する (スケールはx,yの最大値をとる）
	num
};

/**
* αブレンド方法
*/
enum BlendType
{
	BLEND_MIX,		///< 0 ブレンド（ミックス）
	BLEND_MUL,		///< 1 乗算
	BLEND_ADD,		///< 2 加算
	BLEND_SUB		///< 3 減算
};

/**
 * Player
 */
class Player
{
public:
	/**
	 * Playerインスタンスを構築します.
	 *
	 * @param  resman  使用するResourceManagerインスタンス. 省略時はデフォルトインスタンスが使用されます.
	 * @return Playerインスタンス
	 */
	static Player* create(ResourceManager* resman = NULL);

	/**
	 * 使用するResourceManagerインスタンスを設定します.
	 *
	 * @param  resman  使用するResourceManagerインスタンス. 省略時はデフォルトインスタンスが使用されます.
	 */
	void setResourceManager(ResourceManager* resman = NULL);

	/**
	 * 使用中のResourceManagerインスタンスを解放します.
	 * 再度ResourceManagerインスタンスを設定するまでは再生できなくなります.
	 */
	void releaseResourceManager();

	/**
	 * 再生するssbpデータのdataKeyを設定します.
	 *
	 * @param  dataKey  再生するデータのdataKey
	 */
	void setData(const std::string& dataKey);

	/**
	 * 設定されているssbpデータを解放します.
	 */
	void releaseData();

	/**
	 * 設定されているアニメーションを解放します.
	 */
	void releaseAnime();

	/**
	 * アニメーションの再生を開始します.
	 *
	 * @param  packName      パック名(ssae）
	 * @param  animeName     再生するアニメーション名
	 * @param  loop          再生ループ数の指定. 省略時は0
	 * @param  startFrameNo  再生を開始するフレームNoの指定. 省略時は0
	 */
	void play(const std::string& packName, const std::string& animeName, int loop = 0, int startFrameNo = 0);

	/**
	 * アニメーションの再生を開始します.
	 * アニメーション名から再生するデータを選択します.
	 * "ssae名/アニメーション名" という指定が可能です.
	 * sample.ssaeのanime_1を指定する場合、sample/anime_1となります.
	 * ※アニメーション名のみで指定した場合、同名のアニメーションが複数存在時にどのアニメーションが選択されるかは不定です.
	 *
	 * @param  animeName     再生するアニメーション名
	 * @param  loop          再生ループ数の指定. 省略時は0
	 * @param  startFrameNo  再生を開始するフレームNoの指定. 省略時は0
	 */
	void play(const std::string& animeName, int loop = 0, int startFrameNo = 0);

	/**
	 * 再生を中断します.
	 */
	void pause();

	/**
	 * 再生を再開します.
	 */
	void resume();

	/**
	 * 再生を停止します.
	 * ゲーム側でアニメーションの表示フレームを制御する場合はstop()を呼び出した後
	 * ゲーム側の更新処理でsetFrameNo()を呼び出し指定のフレームを表示してください。
	 */
	void stop();

	/**
	 * 再生しているアニメーションのパック名(ssae)を返します.
	 *
	 * @return パック名(ssae)
	 */
	const std::string& getPlayPackName() const;

	/**
	 * 再生しているアニメーション名を返します.
	 *
	 * @return アニメーション名
	 */
	const std::string& getPlayAnimeName() const;
	
	/**
	* アニメーションの総フレームを取得します.
	*
	* @return 総フレーム
	*/
	int getMaxFrame() const;

	/**
	 * 再生フレームNoを取得します.
	 * Get frame no of playing.
	 *
	 * @return 再生フレームNo. frame no.
	 */
	int getFrameNo() const;

	/**
	 * 再生フレームNoを設定します.
	 * Set frame no of playing.
	 *
	 * @param 再生フレームNo. frame no.
	 */
	void setFrameNo(int frameNo);

	/**
	 * 再生スピードを取得します. (1.0f:標準)
	 * Set speed to play. (1.0f:normal speed)
	 */
	float getStep() const;

	/**
	 * 再生スピードを設定します. (1.0f:標準)
	 * Get speed to play. (1.0f:normal speed)
	 */
	void setStep(float step);
	
	/** 
	 * 指定されている再生ループ回数を取得します. (0:指定なし)
	 * Get a playback loop count of specified. (0:not specified)
	 */
	int getLoop() const;

	/** 
	 * 再生ループ回数を設定します. (0:指定なし)
	 * Set a playback loop count.  (0:not specified)
	 */
	void setLoop(int loop);

	/** 
	 * 現在までのループ再生回数を取得します.
	 * Get repeat count a playback.
	 */
	int getLoopCount() const;

	/** 
	 * 現在までのループ再生回数をクリアします.
	 * Clear repeat count a playback.
	 */
	void clearLoopCount();

	/**
	 * フレームスキップ（フレームレートに合わせ再生フレームをスキップする）の設定をします. (default: true)
	 * Set frame-skip(to skip the playback frame according to the frame rate). (default: true)
	 */
	void setFrameSkipEnabled(bool enabled);
	
	/** 
	 * フレームスキップの設定状態を返します.
	 * Get frame-skip setting.
	 */
	bool isFrameSkipEnabled() const;

	/**
	* ラベル名からフレーム位置を取得します.
	*/
	int getLabelToFrame(char* findLabelName);

	/**
	* indexからパーツ名を取得します.
	*/
	const char* getPartName(int partId) const;

	/**
	* パーツ名からindexを取得します.
	*/
	int indexOfPart(const char* partName) const;

	/**
	* パーツの名から、パーツ情報を取得します.
	*/
	bool getPartState(ResluteState& result, const char* name, int frameNo = -1);

	/**
	* パーツの表示、非表示を設定します.
	*/
	void setPartVisible( int partNo, bool flg );

	/* 
	* プレイヤー本体の位置を設定します。
	*/
	void  setPosition(float x, float y);

	/*
	* プレイヤー本体の回転角度を設定します。2Dの回転はZに値を設定してください。
	*/

	void  setRotation(float x, float y, float z);
	/*
	* プレイヤー本体のスケールを設定します。
	*/
	void  setScale(float x, float y);

	/*
	* プレイヤー本体の透明度を設定します。
	*/
	void  setAlpha(int a);

	/*
	* プレイヤー本体の反転を設定します。
	*/
	void  setFlip(bool flipX, bool flipY);

	/*
	* ゲームのフレームレートを設定します。ディフォルトは60FPS。
	*/
	void setGameFPS(float fps);

	/*
	* プレイヤーの更新を行います。ゲームの更新タイミングで呼び出してください。
	*/
	void update(float dt);

	/*
	* プレイヤーの表示を行います。ゲームの表示タイミングで呼び出してください。
	*/
	void draw();

	int getFPS();


public:
	Player(void);
	~Player();
	bool init();


protected:
	void allocParts(int numParts, bool useCustomShaderProgram);
	void releaseParts();
	void setPartsParentage();

	void play(AnimeRef* animeRef, int loop, int startFrameNo);
	void updateFrame(float dt);
	void setFrame(int frameNo);
	void checkUserData(int frameNo);
	void get_uv_rotation(float *u, float *v, float cu, float cv, float deg);
	void set_InstanceAlpha(int alpha);
	void set_InstanceRotation(float rotX, float rotY, float rotZ);

protected:
	ResourceManager*	_resman;
	ResourceSet*		_currentRs;
	std::string			_currentdataKey;
	AnimeRef*			_currentAnimeRef;
	std::vector<CustomSprite *>	_parts;

	bool				_frameSkipEnabled;
	float				_playingFrame;
	float				_step;
	int					_loop;
	int					_loopCount;
	bool				_isPlaying;
	bool				_isPausing;
	int					_prevDrawFrameNo;
	bool				_partVisible[PART_VISIBLE_MAX];
	int					_InstanceAlpha;
	float				_InstanceRotX;
	float				_InstanceRotY;
	float				_InstanceRotZ;

	UserData			_userData;

	State				_state;
	int					_partIndex[PART_VISIBLE_MAX];
	float				_gamefps;
};


};	// namespace ss

#endif
