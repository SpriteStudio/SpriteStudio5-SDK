#ifndef __bind_ANIMATION__
#define __bind_ANIMATION__

#include "package_SpriteStudio.h"
#include "ssplayer_animedecode.h"

#include <boost/python.hpp>
#include <boost/python/numeric.hpp>

//#include "ssplayer_cellmap.h"
//#include "bind_Cellmap.h"

class Bind_Animation {
public:
    Bind_Animation(){ puts("construct Animation"); }
    char const* init(){
        return "Animation init";
    }
};


class Bind_SsCellValue;

class Bind_SsPartState : public myPyBinder<SsPartState>
{
private:

public:
	Bind_SsPartState(){}
	virtual ~Bind_SsPartState(){}

	BIND_RETURN_PROPEX( int , index );

	BIND_RETURN_PROPEX_ARRAY( float , vertices , 5 , 3 );
	BIND_RETURN_PROPEX_ARRAY( float , colors,	4, 4);
	BIND_RETURN_PROPEX_ARRAY( float , uvs ,	5, 2);
	BIND_RETURN_PROPEX_ARRAY( float , matrix,4,4 );
	BIND_RETURN_PROPEX( SsVector3 , position );
	BIND_RETURN_PROPEX( SsVector3 , rotation );
	BIND_RETURN_PROPEX( SsVector2 , scale );
	BIND_RETURN_PROPEX( float , alpha );
	BIND_RETURN_PROPEX( int , prio );
	BIND_RETURN_PROPEX( bool , hFlip );
	BIND_RETURN_PROPEX( bool , vFlip );

	BIND_RETURN_PROPEX( bool , hide );
	BIND_RETURN_PROPEX( SsVector2 , pivotOffset );
	BIND_RETURN_PROPEX( SsVector2 , anchor );
	BIND_RETURN_PROPEX( SsVector2 , size );

	BIND_RETURN_PROPEX( bool , imageFlipH );
	BIND_RETURN_PROPEX( bool , imageFlipV );
	BIND_RETURN_PROPEX( SsVector2 , uvTranslate );

	BIND_RETURN_PROPEX( float , uvRotation );
	BIND_RETURN_PROPEX( SsVector2 , uvScale );

	BIND_RETURN_PROPEX( float , boundingRadius );
	BIND_RETURN_PROPEX( bool , noCells );
	BIND_RETURN_PROPEX( bool , is_color_blend );
	BIND_RETURN_PROPEX( bool , is_vertex_transform );

	BIND_RETURN_PROPEX( int , alphaBlendType );

	BIND_RETURN_PROPEX( SsColorAnime , colorValue );
	BIND_RETURN_PROPEX( SsVertexAnime , vertexValue );


	Bind_SsCellValue*	cellValue();

	//BIND_NEW_PROP_REF(	Bind_SsCellValue , cellValue );

	//以下は別途
//	SsCellValue		cellValue;		///< セルアニメの値
//	SsColorAnime	colorValue;		///< カラーアニメの値
//	SsVertexAnime	vertexValue;	///< 頂点アニメの値

};


#endif
