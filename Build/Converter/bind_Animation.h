#ifndef __bind_ANIMATION__
#define __bind_ANIMATION__


#define BOOST_PYTHON_STATIC_LIB
#include    <stdio.h>
#include    <Python.h>
#include    <boost/python.hpp>

#include	<fstream>
#include	<iostream>
#include	<sstream>

using namespace boost::python;
using namespace std;



class Animation {
public:
    Animation(){ puts("construct Animation"); }
    char const* init(){
        return "Animation init";
    }
};



#endif
