#ifndef __SSEFFECTBEHAVIOR__
#define __SSEFFECTBEHAVIOR__

#include "SsEffectElement.h"


class SsEffectBehavior
{
public:

	//ParticleElementList 	plist;
	//ParticleElementBasic*   basicData;	//元々必要なデータなので別にするかも


public:
    SsCell*					refCell;
	SsString				CellName;
	SsString				CellMapName;
	SsRenderBlendType       blendType;

public:
	SsEffectBehavior(){}
	virtual ~SsEffectBehavior(){}


};






#endif
