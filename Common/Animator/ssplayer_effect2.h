#ifndef __SSEFFECTRENDER_V3__
#define __SSEFFECTRENDER_V3__


#include "xorshift32.h"

//SsVectorなど
//#include "SsUtility.h"

//#include "ISSEffectRender.h"


class SsEffectModel;
class SsRenderEffectBase;
class SsEffectNode;
class SsPartState;
class SsEffectRenderAtom;
class SsCell;



#define SEED_MAGIC (7573)
#define LOOP_TYPE1 (0)
#define LOOP_TYPE2 (0)
#define LOOP_TYPE3 (1)



//v3.1
struct particleExistSt
{
	int  id;
	int	 cycle;
	int	 exist;
    int	 born;
	long stime;
	long endtime;
};


//v3.1
struct emitPattern
{
	int   offsetTime;//0フレームからの開始オフセット
	int   life;
    int   cycle;
};


//最終描画用データ
struct particleDrawData
{
	int	id;
	int	pid;
	int	stime;		//生成された開始時間
	int lifetime;

	//描画用ワーク
	float	x;
	float	y;
	float	rot;
	float	direc;

    SsU8Color color;
    SsVector2 scale;
};



//エミッターが持つパラメータ
//エディタやファイルから入力される
struct emitterParameter
{
	int	life;
	int interval;
	int	emitnum;		//一度に排出される個数
	int emitmax;		//最大排出数
	int	particleLife;	//生成されるパーティクルの寿命
	int particleLife2;	//寿命最大値
    bool Infinite;      //無限発生

	int	loopStart;
	int loopEnd;
	int loopLen;
	int	loopGen;


	//テスト用デフォルト
	emitterParameter() : life(15),interval(1),emitnum(2),emitmax(32),particleLife(15),particleLife2(15),Infinite(false){}

};


//パーティクルが持つパラメータ
//エディタやファイルから入力される
struct particleParameter
{

	SsVector2 	scale;

	SsU8Color   startcolor; //スタート時のカラー
	SsU8Color   endcolor;   //終了時のカラー

	//初速
	float		speed;		//初速
	float		speed2;		//初速最大値


	float		angle;		  //向いてる角度
	float       angleVariance;//変差

	bool		useGravity;
	SsVector2	gravity;


	bool		useOffset;
	SsVector2   offset;
	SsVector2   offset2;

	bool        useRotation;
	float		rotation;
	float		rotation2;

	float		rotationAdd;
	float		rotationAdd2;

	bool		useRotationTrans;
	float		rotationFactor;
	float		endLifeTimePer;

	bool        useTanAccel;
	float		tangentialAccel;
	float		tangentialAccel2;

	bool        useColor;
	SsU8Color   initColor;
	SsU8Color   initColor2;

	bool		useTransColor;
	SsU8Color   transColor;
	SsU8Color   transColor2;

	bool        useInitScale;
	SsVector2   scaleRange;
	float		scaleFactor;
	float		scaleFactor2;

	bool        useTransScale;
	SsVector2   transscale;
	SsVector2   transscaleRange;
	float		transscaleFactor;
	float		transscaleFactor2;

	float		delay;

	bool		usePGravity;
	SsVector2	gravityPos;
	float		gravityPower;

	bool		useAlphaFade;
	float       alphaFade;
	float       alphaFade2;

	bool		useTransSpeed;
	float	   	transSpeed;
	float	   	transSpeed2;

	bool		useTurnDirec;

    bool		userOverrideRSeed;
	int			overrideRSeed;


	particleParameter(){}
};


//エミッタ動作クラス
class SsEffectEmitter
{
public:

	int					priority;

	emitterParameter  	emitter;
	particleParameter   particle;
	xorshift32			rand;


	int				emitterSeed;

	//生成用のリングバッファ
	//particleRingBuffer<particleLifeSt>	_tempbuf;

//	emitPattern*    	_emitpattern;
	std::vector<emitPattern>    	_emitpattern;


    particleExistSt*     particleExistList;


	//事前計算バッファ
	//particleLifeSt*				particleList;
	int							particleIdMax;

	size_t						particleListBufferSize;
    unsigned long*              seedList;


	SsVector2   				position;
//	SsEffectEmitter*			_child;
	SsEffectEmitter*			_parent;



    int							_parentIndex;

	SsCell*						refCell;    //描画用セル
	SsEffectBehavior*           refData;	//データ更新用

	size_t						globaltime;


public:
	SsEffectEmitter() :
//			particleList(0),
			_parentIndex(-1),
			seedList(0),
			particleListBufferSize(180*100),  //生成出来るパーティクルの最大値
			_emitpattern(0),
			particleExistList(0)
	{
		emitterSeed = SEED_MAGIC;
	}
	virtual ~SsEffectEmitter(){}



//	const particleLifeSt*	getParticleDataFromID(int id) { return &particleList[id]; }

#if  LOOP_TYPE3

	int	getParticleIDMax() { return _emitpattern.size(); }
	const 	particleExistSt*	getParticleDataFromID(int id);
	void	updateEmitter( double time );

#else

	int	getParticleIDMax() { return particleIdMax; }
	const particleLifeSt*	getParticleDataFromID(int id);
#endif


	int	getTimeLength() { return emitter.life + ( emitter.particleLife + emitter.particleLife2); }

	//現在時間から産出される位置を求める
	//time変数から求められる式とする
	void	updateParticle(float time, particleDrawData* p, bool recalc = false );

	//パーティクルの発生間隔を事前計算する
	//ここで出力が確定する

	void	precalculate2();

};


class SsEffectRenderV3 //: public ISsEffectRenderer
{
public:

	//エフェクトのパラメータデータ
	SsEffectModel*		effectData;

	//Modelに記載されているエミッタのリスト
	std::vector<SsEffectEmitter*>   emmiterList;

	std::vector<SsEffectEmitter*>   updateList;

	//ランダムシード
	u32				mySeed;

	SsVector3		layoutPosition;

	float			nowFrame;
    float			secondNowFrame;

	size_t          effectTimeLength;

    bool			Infinite;	//無限に発生出来るかどうか

	SsPartState*		parentState;


public:
	//デバッグ用
	int  loopcnt;
	int  drawcnt;


protected:
	void 	particleDraw(SsEffectEmitter* e , double t , SsEffectEmitter* parent = 0 , particleDrawData* plp = 0 );
	void	initEmitter( SsEffectEmitter* e , SsEffectNode* node);

	void	clearEmitterList();



public:
	SsEffectRenderV3() : effectTimeLength(0) {}
	virtual ~SsEffectRenderV3(){}


	virtual void	setEffectData(SsEffectModel* data);


	virtual void	setSeed( u32 seed )
	{
   		mySeed = seed * SEED_MAGIC;
	}


	virtual void	setFrame( float frame )
	{
    	nowFrame = frame;
	}

    virtual float	getFrame(){ return nowFrame; }

	virtual void	update(float delta);
	virtual void	draw();

	virtual void    reload();

    virtual size_t  getEffectTimeLength();

//	virtual int		getVersion(){ return EFFECTRENDERVERSION_V2; }

	virtual  void	setParentAnimeState( SsPartState* state ){ parentState = state; }

	virtual void stop(){}

};


#endif
