#include <stdio.h>
#include <cstdlib>

#include "../loader/ssloader.h"
#include "ssplayer_animedecode.h"
#include "ssplayer_effect.h"
#include "ssplayer_macro.h"
#include "ssplayer_matrix.h"
#include "ssplayer_render.h"
#include "ssplayer_effectfunction.h"


class EffectFuncBase
{
public:
	EffectFuncBase(){}
	virtual ~EffectFuncBase(){}

	virtual void	initalizeEmmiter ( SsEffectElementBase* ele , SsEffectRenderEmitter* emmiter){}
	virtual void	updateEmmiter( SsEffectElementBase* ele , SsEffectRenderEmitter* emmiter){}
	virtual void	initializeParticle( SsEffectElementBase* ele , SsEffectRenderEmitter* e , SsEffectRenderParticle* particle ){}
	virtual void	updateParticle( SsEffectElementBase* ele , SsEffectRenderEmitter* e , SsEffectRenderParticle* particle ){}
};




//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementBasic : public EffectFuncBase
{
public:
	FuncParticleElementBasic(){}
	virtual ~FuncParticleElementBasic(){}

	virtual void	initalizeEmmiter ( SsEffectElementBase* ele , SsEffectRenderEmitter* emmiter)
	{
		ParticleElementBasic* e = static_cast<ParticleElementBasic*>(ele);
	
	}
	virtual void	updateEmmiter( SsEffectElementBase* ele , SsEffectRenderEmitter* emmiter){}
	virtual void	initializeParticle( SsEffectElementBase* ele , SsEffectRenderEmitter* e , SsEffectRenderParticle* particle ){}
	virtual void	updateParticle( SsEffectElementBase* ele , SsEffectRenderEmitter* e , SsEffectRenderParticle* particle ){}
	
};
static FuncParticleElementBasic		funcBasic;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementRndSeedChange : public EffectFuncBase
{
public:
	FuncParticleElementRndSeedChange(){}
	virtual ~FuncParticleElementRndSeedChange(){}
	
};
static FuncParticleElementRndSeedChange		funcRndSeedChange;


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementDelay : public EffectFuncBase
{
public:
	FuncParticleElementDelay(){}
	virtual ~FuncParticleElementDelay(){}
	
};
static FuncParticleElementDelay		funcDelay;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementGravity : public EffectFuncBase
{
public:
	FuncParticleElementGravity(){}
	virtual ~FuncParticleElementGravity(){}
	
};
static FuncParticleElementGravity		funcGravity;


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementPosition : public EffectFuncBase
{
public:
	FuncParticleElementPosition(){}
	virtual ~FuncParticleElementPosition(){}
	
};
static FuncParticleElementPosition		funcPosition;



//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementTransPosition: public EffectFuncBase
{
public:
	FuncParticleElementTransPosition(){}
	virtual ~FuncParticleElementTransPosition(){}
	
};
static FuncParticleElementPosition		funcTransPosition;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementRotation: public EffectFuncBase
{
public:
	FuncParticleElementRotation(){}
	virtual ~FuncParticleElementRotation(){}
	
};
static FuncParticleElementRotation		funcRotation ;



//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementRotationTrans : public EffectFuncBase
{
public:
	FuncParticleElementRotationTrans(){}
	virtual ~FuncParticleElementRotationTrans(){}
	
};
static FuncParticleElementRotationTrans		funcRotationTrans ;


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementTransSpeed : public EffectFuncBase
{
public:
	FuncParticleElementTransSpeed(){}
	virtual ~FuncParticleElementTransSpeed(){}
	
};
static FuncParticleElementTransSpeed		funcTransSpeed ;


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementTangentialAcceleration : public EffectFuncBase
{
public:
	FuncParticleElementTangentialAcceleration(){}
	virtual ~FuncParticleElementTangentialAcceleration(){}
	
};
static FuncParticleElementTangentialAcceleration		funcTangentialAcceleration ;


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementInitColor : public EffectFuncBase
{
public:
	FuncParticleElementInitColor(){}
	virtual ~FuncParticleElementInitColor(){}
	
};
static FuncParticleElementInitColor		funcInitColor;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementTransColor : public EffectFuncBase
{
public:
	FuncParticleElementTransColor(){}
	virtual ~FuncParticleElementTransColor(){}
	
};
static FuncParticleElementTransColor		funcTransColor;


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementAlphaFade : public EffectFuncBase
{
public:
	FuncParticleElementAlphaFade(){}
	virtual ~FuncParticleElementAlphaFade(){}
	
};
static FuncParticleElementAlphaFade		funcAlphaFade;



//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementSize : public EffectFuncBase
{
public:
	FuncParticleElementSize(){}
	virtual ~FuncParticleElementSize(){}
	
};
static FuncParticleElementSize		funcSize;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleElementTransSize : public EffectFuncBase
{
public:
	FuncParticleElementTransSize(){}
	virtual ~FuncParticleElementTransSize(){}
	
};
static FuncParticleElementTransSize		funcTransSize;


//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticlePointGravity : public EffectFuncBase
{
public:
	FuncParticlePointGravity(){}
	virtual ~FuncParticlePointGravity(){}
	
};
static FuncParticlePointGravity		funcPointGravity;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FuncParticleTurnToDirectionEnabled : public EffectFuncBase
{
public:
	FuncParticleTurnToDirectionEnabled(){}
	virtual ~FuncParticleTurnToDirectionEnabled(){}
	
};
static FuncParticleTurnToDirectionEnabled		funcTurnToDirectionEnabled;


//-------------------------------------------------------------------
//挙動反映クラスの呼び出しテーブル
//SsEffectFunctionTypeの順に並べること
//-------------------------------------------------------------------
static EffectFuncBase* callTable[] =
{
	0,
	&funcBasic,
	&funcRndSeedChange,
	&funcDelay,
	&funcGravity,
	&funcPosition,
	&funcTransPosition,
	&funcRotation,
	&funcRotationTrans,
	&funcTransSpeed,
	&funcTangentialAcceleration,
	&funcInitColor,
	&funcTransColor,
	&funcAlphaFade,
	&funcSize,
	&funcTransSize,
	&funcPointGravity,
	&funcTurnToDirectionEnabled,
};






///----------------------------------------------------------------------------------------------------
//
///----------------------------------------------------------------------------------------------------
void	SsEffectFunctionExecuter::initalize( SsEffectBehavior* beh ,  SsEffectRenderEmitter* emmiter)
{
	foreach( std::vector<SsEffectElementBase* > , beh->plist , e )
	{
		EffectFuncBase* cf = callTable[(*e)->myType];
		cf->initalizeEmmiter( (*e) , emmiter );
	}
}

void	SsEffectFunctionExecuter::updateEmmiter( SsEffectBehavior* beh , SsEffectRenderEmitter* emmiter)
{
	foreach( std::vector<SsEffectElementBase* > , beh->plist , e )
	{
		EffectFuncBase* cf = callTable[(*e)->myType];
		cf->updateEmmiter( (*e) , emmiter );
	}
}

void	SsEffectFunctionExecuter::initializeParticle( SsEffectBehavior* beh ,  SsEffectRenderEmitter* emmiter , SsEffectRenderParticle* particle )
{
	foreach( std::vector<SsEffectElementBase* > , beh->plist , e )
	{
		EffectFuncBase* cf = callTable[(*e)->myType];
		cf->initializeParticle( (*e) , emmiter , particle );
	}
}

void	SsEffectFunctionExecuter::updateParticle(  SsEffectBehavior* beh , SsEffectRenderEmitter* emmiter , SsEffectRenderParticle* particle )
{
	foreach( std::vector<SsEffectElementBase* > , beh->plist , e )
	{
		EffectFuncBase* cf = callTable[(*e)->myType];
		cf->updateParticle( (*e) , emmiter , particle );
	}
}



















