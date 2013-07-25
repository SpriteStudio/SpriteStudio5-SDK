#ifndef __TKSCENE__
#define __TKSCENE__


#include "tkObject.h"

///シーングラフを構築するためのクラス
///色々オミットする シーンを既定できればOK
class	tkScene : public tkObject
{
public:
	tkScene(){}
	virtual ~tkScene(){}

	virtual void	draw(){};
	virtual void	update(){};
	virtual void	init(){};
};




#endif

