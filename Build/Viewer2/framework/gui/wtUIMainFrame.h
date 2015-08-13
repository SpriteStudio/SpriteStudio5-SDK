#ifndef __WTUIMAINFRAME__
#define __WTUIMAINFRAME__

#include "wtuiobject.h"


struct NVGcontext;
class wtUIMainFrame : public wtUIObject
{
private:
	NVGcontext* vg;

public:
	wtUIMainFrame(){}
	virtual ~wtUIMainFrame(){}


	int	start();

	void	execute();
	virtual void	draw();

};



#endif