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





#include "ssloader.h"


#include "bind_Animation.h"
#include "bind_SsAnimeDecoder.h"
#include "bind_SsAttribute.h"
#include "bind_AnimePack.h"
#include "bind_SsPart.h"
#include "bind_SsPartAnime.h"
#include "bind_SSXML.h"

#include "bind_SsKeyframe.h"


#endif