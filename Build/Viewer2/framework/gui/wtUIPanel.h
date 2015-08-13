#ifndef __WTUIPANEL__
#define __WTUIPANEL__

#include "wtuiobject.h"


class wtUIPanel : public wtUIObject
{
private:
	float x;
	float y;
	float w;
	float h;

public:
	wtUIPanel() :
		x(0),y(0),w(100),h(100)
	{}
	virtual ~wtUIPanel(){}


	virtual void	draw();


	virtual void	before_update(){}
	virtual void	update(){}
	virtual void	after_update(){}

};



#endif