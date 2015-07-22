#ifndef __SSLOADER_SSEE__

#include "sstypes.h"
#include "ssarchiver.h"

#include "effect/SsEffectBehavior.h"



class SimpleTree
{
public:
	SimpleTree *parent;
	SimpleTree *ctop;
	SimpleTree *prev;
	SimpleTree *next;

public:
	SimpleTree()
		: parent(0),ctop(0),prev(0),next(0)
	{}
	~SimpleTree()
	{
		destroy();
	}

	void	addChildEnd( SimpleTree* c ){ 
		if ( ctop == 0 )
		{
			ctop = c; 
		}else{
			ctop->addSiblingEnd( c );
		}
		c->parent = this;
	}
	void	addSiblingEnd( SimpleTree* c )
	{
		if ( next == 0 )
		{
			c->prev = this;
			next = c;
		}else{
			next->addSiblingEnd(c);
		}

		c->parent = this->parent;
	}

	void destroysub(SimpleTree *t)
	{
		if ( t->ctop )
		{
			destroysub(t->ctop);
		}
		if ( t->next )
		{
			destroysub(t->next);
		}

		this->ctop = 0;
		this->next = 0;
		this->prev = 0;
		delete this;
	}
	void destroy()
	{
		destroysub(this);	
	}
};



enum SsEffectNodeType{
	SsEffectNodeType_Root,
	SsEffectNodeType_Emmiter,
	SsEffectNodeType_Particle,
};




class SsEffectNode : public SimpleTree
{
public:
	int					arrayIndex;
	int					parentIndex;
	SsEffectNodeType	type;

	SsEffectBehavior*	myBehavior;

public:
	SsEffectNode(){}
	~SsEffectNode(){}
};


class SsEffectModel
{
private:
	SsEffectNode* root;

public:
	int			lockRandSeed; 	 // ランダムシード固定値
	bool    	isLockRandSeed;  // ランダムシードを固定するか否か
	int			fps;             //
	SsU8Color   bgcolor;

public:
	SsEffectModel()
	{}

	virtual ~SsEffectModel(){
		delete root;
		root = 0;
	}

    //アクセス
	SsEffectNode* GetRoot(){ return root;}


	//シリアライザ
	SSSERIALIZE_BLOCK
	{
		SSAR_DECLARE(lockRandSeed);
		SSAR_DECLARE(isLockRandSeed);
		SSAR_DECLARE(fps);
		//SSAR_DECLARE(bgcolor); //bgcolorはとりこまない

		EffectNodeLoader(ar);
	}

	void	EffectNodeLoader(ISsXmlArchiver* ar);


};




class SsEffectFile
{
public:
	SsEffectModel	   effectData;  //親子構造＋各アトリビュート


	SsEffectFile(){}
	virtual ~SsEffectFile(){}

	SSSERIALIZE_BLOCK
	{
		SSAR_STRUCT_DECLARE( effectData );
	}

};


//!sspjのローダークラスです。
class ssloader_ssee
{
public:
	ssloader_ssee(){}
	virtual ~ssloader_ssee(){}

	static SsEffectFile*	Load(const std::string& filename );

};

#endif
