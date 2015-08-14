#ifndef __WTUIPANEL__
#define __WTUIPANEL__

#include "wtuiobject.h"

namespace wtUI
{
	namespace Dock
	{
		enum _enum
		{
			none,
			left,
			right,
			top,
			bottom,
		};
	}

	namespace Align
	{
		enum _enum{
			none,
			left,
			right,
			center,
			full,
		};
	};
};


class wtUIPanel : public wtUIObject
{
private:

	SsPoint2	drawPosition;
	int			drawWidth;
	int			drawHeight;

public:
	SsPoint2	position;
	int			width;
	int			height;
	SsString	caption;

	wtUI::Dock::_enum	DockStatus;
	wtUI::Align::_enum	AlignStatus;

public:
	wtUIPanel() :
	  position(0,0),width(300),height(500) , 
		  DockStatus(wtUI::Dock::left),caption("window"),AlignStatus(wtUI::Align::none)
	{}
	virtual ~wtUIPanel(){}
	virtual void	draw();

	virtual void	before_update(){}
	virtual void	update();
	virtual void	after_update(){}
	virtual void	resize( int x , int y , int w , int h );

};



#endif