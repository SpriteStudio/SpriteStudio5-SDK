#ifndef __GLHELPER__
#define __GLHELPER__

#include <GLFW/glfw3.h>
#include "../../loader/ssloader.h"


#include "tkObject.h"
#include "tkTask.h"
#include "tkScene.h"
#include "tkTexture.h"

void DrawSprite(float x, float y, float width, float height, SsVector2 uv1,
	SsVector2 uv2, const SsFColor& color);


// nが2のべき乗かどうかチェック
inline bool SsGL_isPow2(int n)
{

	for (int i = 0; i < 30; i++)
	{
		if (n == (1 << i))
		{
			return true;
		}
	}
	return false;
}



#endif
