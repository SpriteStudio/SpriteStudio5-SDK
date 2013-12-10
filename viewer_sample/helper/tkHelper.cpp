#include "tkHelper.h"


// ---------------------------------------------------------------------------
void DrawSprite(float x, float y, float width, float height, SsVector2 uv1,
	SsVector2 uv2, const SsFColor& color) {

	glEnable(GL_BLEND);
	glBegin(GL_QUADS);
	glColor4f(color.r, color.g, color.b, color.a);

	glTexCoord2d(uv1.x, uv1.y);
	glVertex2f(x, y);
	glTexCoord2d(uv1.x, uv2.y);
	glVertex2f(x, y - height);
	glTexCoord2d(uv2.x, uv2.y);
	glVertex2f(x + width, y - height);
	glTexCoord2d(uv2.x, uv1.y);
	glVertex2f(x + width, y);
	glEnd();
	glDisable(GL_BLEND);

}

