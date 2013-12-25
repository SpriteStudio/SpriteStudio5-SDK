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


#include "ssloader.h"


#include "bind_Animation.h"
#include "bind_SsAnimeDecoder.h"
#include "bind_AnimePack.h"
#include "bind_SSXML.h"


void PYDEBUG_PRINTF( const char* strFormat, ...   );



#endif