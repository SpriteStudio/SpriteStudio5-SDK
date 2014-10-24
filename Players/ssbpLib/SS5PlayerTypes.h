/**
*  SS5PlayerTypes.h
*/

#ifndef SS5PlayerTypes_h
#define SS5PlayerTypes_h

namespace ss
{
/**
* カラー構造体
*/
typedef struct _SSColor4B
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} SSColor4B;

/**
* 頂点座標構造体
*/
typedef struct _SSVertex3F
{
	float x;
	float y;
	float z;
} SSVertex3F;

/**
* ４頂点座標構造体
*/
typedef struct _SSQuad3 {
	SSVertex3F        bl;
	SSVertex3F        br;
	SSVertex3F        tl;
	SSVertex3F        tr;
} SSQuad3;

/**
* UV構造体
*/
typedef struct _SSTex2F {
	float u;
	float v;
} SSTex2F;

/**
* 頂点構造体
*/
typedef struct _ccV3F_C4B_T2F
{
	SSVertex3F			vertices;	/// 座標
	SSColor4B			colors;		/// カラー
	SSTex2F				texCoords;	/// UV
} SSV3F_C4B_T2F;

/**
* ４頂点構造体
*/
typedef struct _SSV3F_C4B_T2F_Quad
{
	SSV3F_C4B_T2F    tl;
	SSV3F_C4B_T2F    bl;
	SSV3F_C4B_T2F    tr;
	SSV3F_C4B_T2F    br;
} SSV3F_C4B_T2F_Quad;



};	// namespace ss

#endif
