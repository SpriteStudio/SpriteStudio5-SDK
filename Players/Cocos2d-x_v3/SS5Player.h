//
//  SS5Player.h
//


/************************************************************

- Quick start
 
  #include "SS5Player.h"
 
  auto resman = ss::ResourceManager::getInstance();
  resman->addData("sample.ssbp");

  auto player = ss::Player::create();
  player->setData("sample");			// ssbpファイル名（拡張子不要）
  player->play("anime1");
  player->setPosition(200, 200);
  this->addChild(player);

  X回転、Y回転を使用する場合、アプリケーションの初期化で平行投影を設定しないとSS5と描画結果が異なります。
  Z回転とXまたはY回転を同時に行うとSS5と描画結果が異なります。
  //平行投影の設定
  director->setProjection(kCCDirectorProjection2D);


*************************************************************/


#ifndef SS5Player_h
#define SS5Player_h

#include "cocos2d.h"


namespace ss
{

class ResourceSet;
class AnimeRef;
struct ProjectData;

/**
 * ResourceManager
 */
class ResourceManager : public cocos2d::Ref
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

	ResourceSet* getData(const std::string& dataKey);

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
	cocos2d::Map<std::string, ResourceSet*>	_dataDic;
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


#define PART_VISIBLE_MAX (512)
/**
 * Player
 */
class Player : public cocos2d::Sprite
{
public:

	/**
	 * Playerインスタンスを構築します.
	 *
	 * @param  resman  使用するResourceManagerインスタンス. 省略時はデフォルトインスタンスが使用されます.
	 * @return Playerインスタンス
	 */
	static Player* create(ResourceManager* resman = nullptr);

	/**
	 * 使用するResourceManagerインスタンスを設定します.
	 *
	 * @param  resman  使用するResourceManagerインスタンス. 省略時はデフォルトインスタンスが使用されます.
	 */
	void setResourceManager(ResourceManager* resman = nullptr);

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
	* パーツの表示、非表示を設定します.
	*/
	void setPartVisible( int partNo, bool flg );

	typedef std::function<void(Player*, const UserData*)> UserDataCallback;
	typedef std::function<void(Player*)> PlayEndCallback;

	/** 
	 * ユーザーデータを受け取るコールバックを設定します.
	 *
	 * @param  callback  ユーザーデータ受け取りコールバック
	 *
     * @code
	 * player->setUserDataCallback(CC_CALLBACK_2(MyScene::userDataCallback, this));
	 * --
	 * void MyScene::userDataCallback(ss::Player* player, const ss::UserData* data)
	 * {
	 *   ...
	 * }
     * @endcode
	 */
	void setUserDataCallback(const UserDataCallback& callback);

	/**
	 * 再生終了時に呼び出されるコールバックを設定します.
	 *
	 * @param  callback  ユーザーデータ受け取りコールバック
	 *
     * @code
	 * player->setPlayEndCallback(CC_CALLBACK_1(MyScene::playEndCallback, this));
	 * --
	 * void MyScene::playEndCallback(ss::Player* player)
	 * {
	 *   ...
	 * }
     * @endcode
	 */
	void setPlayEndCallback(const PlayEndCallback& callback);



public:
	Player(void);
	virtual ~Player();

	// override
	virtual bool init();
	virtual void update(float dt);
	virtual void setGlobalZOrder(float globalZOrder);

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
	cocos2d::Vector<cocos2d::Sprite*>	_parts;

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

	
	UserDataCallback	_userDataCallback;
	UserData			_userData;
	PlayEndCallback		_playEndCallback;
};



};	// namespace ss

#endif
