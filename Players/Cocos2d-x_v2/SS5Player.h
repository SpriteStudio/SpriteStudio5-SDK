// 
//  SS5Player.h
//


/************************************************************

- Quick start
 
  #include "SS5Player.h"
 
  ss::ResourceManager* resman = ss::ResourceManager::getInstance();
  resman->addData("sample.ssbp");		// ssbpの読み込み

  ss::Player* ssplayer = ss::Player::create();
  ssplayer->setData("sample");			// ssbpファイル（拡張子不要）をプレイヤーに関連づけます
  ssplayer->play("anime1");				// アニメーション名指定(ssae名/アニメーション名)
  CCPoint pos(200,200);
  ssplayer->setPosition(pos);			// 位置設定
  ssplayer->setAlpha(128);				// 透明度設定
  ssplayer->setScaleX(1.0f);			// X拡大率設定
  ssplayer->setScaleY(1.0f);			// Y拡大率設定
  ssplayer->setRotation(0.0f);			// Z回転値設定(度)
  this->addChild(ssplayer);

  SS5Player cocos2d-x ver2.x版は終了処理で resman を delete してください。

  ver2.X系のSS5PlayerはX回転、Y回転に非対応です。

*************************************************************/


#ifndef SS5Player_h
#define SS5Player_h

#include "cocos2d.h"
#include "SS5PlayerData.h"


namespace ss
{
class SSPlayerDelegate;
class CustomSprite;
class CellCache;
class CellRef;
class AnimeCache;
class AnimeRef;
class ResourceSet;
struct ProjectData;

/**
 * ResourceManager
 */
//class ResourceManager : public cocos2d::Ref
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
	 *                       解像度によって画像ファイルを差し替える時などに使用してください。
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
	 * パス、拡張子を除いたssbp名を指定してください。
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
	* @param  texture        変更後のテクスチャ
	*                        テクスチャはポインタを参照するだけなので、使用する側で実体を管理する必要があります。
	* @return 変更を行ったか
	*/
	bool changeTexture(char* dataName, char* callName, cocos2d::CCTexture2D* texture);

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
//	cocos2d::Map<std::string, ResourceSet*>	_dataDic;
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

	const char*	partName;		// Part name
	int			frameNo;		// Frame no

	int			flags;			// Flags of valid data
	int			integer;		// Integer
	int			rect[4];		// Rectangle Left, Top, Right, Bottom
	int			point[2];		// Position X, Y
	const char*	str;			// String (zero terminated)
	int			strSize;		// String size (byte count)
};


/**
* LabelData
*/
struct LabelData
{
	std::string	str;			// String (zero terminated)
	int			strSize;		// String size (byte count)
	int			frameNo;		// Frame no
};

/**
* ResluteState
*/
struct ResluteState
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

	int	part_type;					//パーツ種別
	int	part_boundsType;			//当たり判定種類
	int	part_alphaBlendType;		// BlendType
};

//含まれるパーツデータフラグ
enum {
	PART_FLAG_INVISIBLE			= 1 << 0,
	PART_FLAG_FLIP_H			= 1 << 1,
	PART_FLAG_FLIP_V			= 1 << 2,

	// optional parameter flags
	PART_FLAG_CELL_INDEX		= 1 << 3,
	PART_FLAG_POSITION_X		= 1 << 4,
	PART_FLAG_POSITION_Y		= 1 << 5,
	PART_FLAG_POSITION_Z		= 1 << 6,
	PART_FLAG_ANCHOR_X			= 1 << 7,
	PART_FLAG_ANCHOR_Y			= 1 << 8,
	PART_FLAG_ROTATIONX			= 1 << 9,
	PART_FLAG_ROTATIONY			= 1 << 10,
	PART_FLAG_ROTATIONZ			= 1 << 11,
	PART_FLAG_SCALE_X			= 1 << 12,
	PART_FLAG_SCALE_Y			= 1 << 13,
	PART_FLAG_OPACITY			= 1 << 14,
	PART_FLAG_COLOR_BLEND		= 1 << 15,
	PART_FLAG_VERTEX_TRANSFORM	= 1 << 16,

	PART_FLAG_SIZE_X			= 1 << 17,
	PART_FLAG_SIZE_Y			= 1 << 18,

	PART_FLAG_U_MOVE			= 1 << 19,
	PART_FLAG_V_MOVE			= 1 << 20,
	PART_FLAG_UV_ROTATION		= 1 << 21,
	PART_FLAG_U_SCALE			= 1 << 22,
	PART_FLAG_V_SCALE			= 1 << 23,
	PART_FLAG_BOUNDINGRADIUS	= 1 << 24,

	PART_FLAG_INSTANCE_KEYFRAME = 1 << 25,
	PART_FLAG_INSTANCE_START	= 1 << 26,
	PART_FLAG_INSTANCE_END		= 1 << 27,
	PART_FLAG_INSTANCE_SPEED	= 1 << 28,
	PART_FLAG_INSTANCE_LOOP		= 1 << 29,
	PART_FLAG_INSTANCE_LOOP_FLG = 1 << 30,

	NUM_PART_FLAGS
};

//頂点変形フラグ
enum {
	VERTEX_FLAG_LT = 1 << 0,
	VERTEX_FLAG_RT = 1 << 1,
	VERTEX_FLAG_LB = 1 << 2,
	VERTEX_FLAG_RB = 1 << 3,
	VERTEX_FLAG_ONE = 1 << 4	// color blend only
};

//インスタンスフラグ
enum {
	INSTANCE_LOOP_FLAG_INFINITY = 1 << 0,
	INSTANCE_LOOP_FLAG_REVERSE = 1 << 1,
	INSTANCE_LOOP_FLAG_PINGPONG = 1 << 2,
	INSTANCE_LOOP_FLAG_INDEPENDENT = 1 << 3,
};

/// Animation Part Type
enum
{
	PARTTYPE_INVALID = -1,
	PARTTYPE_NULL,			// null。領域を持たずSRT情報のみ。ただし円形の当たり判定は設定可能。
	PARTTYPE_NORMAL,		// 通常パーツ。領域を持つ。画像は無くてもいい。
	PARTTYPE_TEXT,			// テキスト(予約　未実装）
	PARTTYPE_INSTANCE,		// インスタンス。他アニメ、パーツへの参照。シーン編集モードの代替になるもの
	PARTTYPE_NUM
};

//当たり判定の種類
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

//αブレンド方法
enum BlendType
{
	BLEND_MIX,		///< 0 ブレンド（ミックス）
	BLEND_MUL,		///< 1 乗算
	BLEND_ADD,		///< 2 加算
	BLEND_SUB		///< 3 減算
};

//固定少数の定数 10=1ドット
#define DOT (10.0f)
#define PART_VISIBLE_MAX (512)
/**
 * Player
 */
class Player : public cocos2d::CCSprite
{
public:
	typedef void (cocos2d::CCObject::*SEL_PlayEndHandler)(Player*);

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
	* プレイヤーの透明度を設定します(0～255).
	* setOpacityではなくこちらを使用してください。
	*/
	void setAlpha(int alpha);

	/*
	* setContentScaleFactorの数値に合わせて内部のUV補正を有効にするか設定します。
	* マルチ解像度テクスチャ対応を行う際にプレイヤーの画像はそのまま使用する場合は、trueを設定してプレイヤー内UV値を変更してください.
	* 画像を差し替える場合はaddDataの第二引数でパスを指定し、解像度の違うテクスチャを読み込んでください.
	*/
	void setContentScaleEneble(bool eneble);

	/*
	* 名前を指定してパーツの再生するインスタンスアニメを変更します。
	* 指定したパーツがインスタンスパーツでない場合、falseを返します.
	* 再生するアニメの名前は"ssae名/アニメーション名"として再生してください。
	* 現在再生しているアニメを指定することは入れ子となり無限ループとなるためできません。
	*/
	bool changeInstanceAnime(std::string partsname, std::string animeName);


	/** ユーザーデータなどの通知を受け取る、デリゲートを設定します.
	 *  Set delegate. receive a notification, such as user data.
	 *
	 *  @code
	 *  player->setDelegate((SSPlayerDelegate *)this);
	 *  --
	 *
	 *  void MyScene::onUserData(ss::Player* player, const ss::UserData* data)
	 *  {
	 *    ...
	 *  }
	 *  @endcode
	 */
	void setDelegate(SSPlayerDelegate* delegate);
    
	/** 再生終了の通知を受けるコールバックを設定します.
	 *
     *  @code
	 *  player->setPlayEndCallback(this, ssplayer_playend_selector(MyScene::playEndCallback));
	 *  --
	 *  void MyScene::playEndCallback(ss::Player* player)
	 *  {
	 *    ...
	 *  }
     *  @endcode
	 */
	void setPlayEndCallback(cocos2d::CCObject* target, SEL_PlayEndHandler selector);
    



public:
	Player(void);
	virtual ~Player();

	// override
	virtual bool init();
	virtual void update(float dt);

protected:
	void allocParts(int numParts, bool useCustomShaderProgram);
	void releaseParts();
	void setPartsParentage();

	void play(AnimeRef* animeRef, int loop, int startFrameNo);
	void updateFrame(float dt);
	void setFrame(int frameNo);
	void checkUserData(int frameNo);
	void get_uv_rotation(float *u, float *v, float cu, float cv, float deg);
	void set_InstanceRotation(float rotX, float rotY, float rotZ);

protected:
	ResourceManager*	_resman;
	ResourceSet*		_currentRs;
	std::string			_currentdataKey;
	std::string			_currentAnimename;
	AnimeRef*			_currentAnimeRef;
	std::vector<CustomSprite *>	_parts;

	bool				_frameSkipEnabled;
	float				_playingFrame;
	float				_step;
	int					_loop;
	int					_loopCount;
	bool				_isPlaying;
	bool				_isPausing;
	bool				_isPlayFirstUserdataChack;
	bool				_isContentScaleFactorAuto;
	int					_prevDrawFrameNo;
	bool				_partVisible[PART_VISIBLE_MAX];
	int					_partIndex[PART_VISIBLE_MAX];
	int					_InstanceAlpha;
	float				_InstanceRotX;
	float				_InstanceRotY;
	float				_InstanceRotZ;
	int					_animefps;

	SSPlayerDelegate*	_delegate;
	UserData			_userData;
    CCObject*			_playEndTarget;
	SEL_PlayEndHandler	_playEndSelector;
};

#define ssplayer_playend_selector(_SELECTOR) (ss::Player::SEL_PlayEndHandler)(&_SELECTOR)

/**
 * SSPlayerDelegate
 */

class SSPlayerDelegate
{
public:
    SSPlayerDelegate(void) {}
    virtual ~SSPlayerDelegate();
    
    /** ユーザーデータの受信
     *  Receive a user data.
     */
	virtual void onUserData(ss::Player* player, const ss::UserData* data);
};


};	// namespace ss

#endif
