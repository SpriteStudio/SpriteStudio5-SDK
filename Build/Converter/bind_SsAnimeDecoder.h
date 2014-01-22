#ifndef __bind_ANIMESTATE__
#define __bind_ANIMESTATE__


#include "package_SpriteStudio.h"


class Bind_SsPart
{
private:
public:
	SsPart*	m_sspart;	//実態は別にあるのでリリースとかしない

	Bind_SsPart(SsPart* p){ m_sspart = p;}
	Bind_SsPart(){}
	virtual ~Bind_SsPart(){}

	bool	debug();
	
	//SsPartのメンバーを操作する
	const char*	name(){ return m_sspart->name.c_str(); }
	int			arrayIndex(){return m_sspart->arrayIndex; }
	int			parentIndex(){return m_sspart->parentIndex; }
	int			type(){return (int)m_sspart->type; }
	int			boundsType(){return (int)m_sspart->boundsType; }
	int			inheritType(){return (int)m_sspart->inheritType; }
	int			alphaBlendType(){return (int)m_sspart->alphaBlendType; }
	int			show(){return (int)m_sspart->show; }
	int			locked(){return (int)m_sspart->locked; }
	float		inheritRates(int type){ return (float)m_sspart->inheritRates[type]; }
};


class SsAnimeDecoder;
class Bind_SsAnimeDecoder
{
private:

public:
	SsAnimeDecoder*	m_decoder;

	Bind_SsAnimeDecoder();
	virtual ~Bind_SsAnimeDecoder();
	bool debug();

	void setFrame( int f );
	void update();

	Bind_SsPart*	getPart(int index);
	int				getPartNum();


};



#endif
