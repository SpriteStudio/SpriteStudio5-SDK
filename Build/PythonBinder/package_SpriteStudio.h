#ifndef __PACKAGE_SPRITESTUDIO__
#define __PACKAGE_SPRITESTUDIO__

#define BOOST_PYTHON_STATIC_LIB
#include    <stdio.h>
#include    <Python.h>
#include    <boost/python.hpp>

#include	<fstream>
#include	<iostream>
#include	<sstream>

using namespace boost::python;
using namespace std;





void PYDEBUG_PRINTF( const char* strFormat, ...   );


template<class myclass>
class	myPyBinder
{
public:
	myclass*	bind_inst;

	myPyBinder(){}

	virtual ~myPyBinder(){}
	virtual bool debug(){ return true; }
	void	bind(myclass* c) { bind_inst = c;}
	bool	isNull(){ return bind_inst ==0 ; }
};



#define	BIND_RETURN_PROP( _a ) this->bind_inst->_a
#define	BIND_RETURN_PROPEX( type, _a ) type _a() { return (type)this->bind_inst->_a; }
#define	BIND_NEW_PROP( type, _a ) \
	type* _a() { \
		type* new_type = new type();\
		new_type->bind(bind_inst->_a);\
		return new_type;\
	}\


#define	BIND_NEW_PROP_REF( type, _a ) \
	type* _a() { \
		type* new_type = new type();\
		new_type->bind(&bind_inst->_a);\
		return new_type;\
	}\


#if 1 

inline void	_bp_array_resize( boost::python::numeric::array& _array , int x , int  y)
{
	if ( y == 0 )
	{
		_array.resize( x );
	}else{
		_array.resize( boost::python::make_tuple(x, y) );
	}
}

#define	BIND_RETURN_PROPEX_ARRAY( type , _a ,_dx,_dy) \
	boost::python::numeric::array _a() { \
		std::vector<type> array_;\
		size_t len = sizeof(this->bind_inst->_a) / sizeof(type);\
		for ( size_t i = 0 ; i < len ; i++ )\
		{\
			array_.push_back(this->bind_inst->_a[i]);\
		}\
		boost::python::numeric::array _aa(array_);\
		_bp_array_resize( _aa , _dx , _dy );\
		return _aa; }\

#else

#define	BIND_RETURN_PROPEX_ARRAY( type , _a ) \
	boost::python::numeric::array _a() { \
		boost::python::numeric::array _aa(this->bind_inst->_a);\
		_aa.resize( 12 );\
		return _aa; }\

#endif


#include "ssloader.h"


#include "bind_Animation.h"
#include "bind_SsAnimeDecoder.h"
#include "bind_SsAttribute.h"
#include "bind_AnimePack.h"
#include "bind_SsPart.h"
#include "bind_SsPartAnime.h"
#include "bind_SSXML.h"

#include "bind_SsKeyframe.h"
#include "bind_Cellmap.h"


#endif