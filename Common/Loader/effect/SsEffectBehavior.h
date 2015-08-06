#ifndef __SSEFFECTBEHAVIOR__
#define __SSEFFECTBEHAVIOR__

#include "SsEffectElement.h"
#include "sstypes.h"
#include "ssvalue.h"


class SsEffectBehaviorValue
{
	SsValue value;
public:
	SsEffectBehaviorValue(){}
	virtual ~SsEffectBehaviorValue(){}

	SSSERIALIZE_BLOCK
	{
		SsValueSeriarizer( ar , value );
	}
};



class SsEffectBehavior
{
public:

	//ParticleElementList 	plist;
	//std::vector<SsEffectBehaviorValue*> plist;
	std::vector<SsValue*> plist;


public:
    SsCell*							refCell;
	SsString						CellName;
	SsString						CellMapName;
	SsRenderBlendType::_enum		blendType;

public:
	SsEffectBehavior(){}
	virtual ~SsEffectBehavior(){}

	SSSERIALIZE_BLOCK
	{
		SSAR_DECLARE(CellName);
		SSAR_DECLARE(CellMapName);
		SSAR_DECLARE_ENUM( blendType );

		if ( ar->getxml() )
			SSAR_DECLARE_LIST2( plist , "list" );

//		SSAR_DECLARE_LIST( plist );
//		if ( ar->getxml() )
//			EffectElementLoader( ar );
	}

	void	EffectElementLoader(ISsXmlArchiver* ar)
	{
		SsXmlIArchiver list_ar( ar , "list" );



		

		SsValue value;
		SsValueSeriarizer( &list_ar , value );
		//plist.push_back(value);


	}

};






#endif
