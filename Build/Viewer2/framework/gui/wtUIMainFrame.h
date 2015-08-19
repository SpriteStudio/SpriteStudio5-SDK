#ifndef __WTUIMAINFRAME__
#define __WTUIMAINFRAME__

#include "wtuiobject.h"


struct NVGcontext;
class wtUIMainFrame : public wtUIObject
{
private:
	int fontNormal;
	int	fontBold;
	int	fontIcons;
	int	fontJapanease; 

public:
	NVGcontext* vg;

	wtUIMainFrame(){}
	virtual ~wtUIMainFrame(){}


	int	start();

	void	execute();
	virtual void	execdraw();
	virtual void	onCreate();
	virtual	void	onResize(int w, int h);

	virtual void	resize( int x , int y , int w , int h );



};



#endif