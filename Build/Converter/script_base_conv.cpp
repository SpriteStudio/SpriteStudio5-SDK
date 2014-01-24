//

#define BOOST_PYTHON_STATIC_LIB
#include    <stdio.h>
#include    <Python.h>
#include    <boost/python.hpp>

#include	<fstream>
#include	<iostream>
#include	<sstream>

using namespace boost::python;
using namespace std;

#include "package_SpriteStudio.h"

#include "sshTextureBMP.h"


//Pythonモジュールの設定
BOOST_PYTHON_MODULE(SpriteStudio)
{
	using namespace boost::python;

	

    class_<Bind_SsKeyframe>("SsKeyframe")
		.def("debug" , &Bind_SsKeyframe::debug )
		.def("time" , &Bind_SsKeyframe::time )
		;

    class_<Bind_SsAttribute>("SsAttribute")
		.def("debug" , &Bind_SsAttribute::debug )
		.def("firstKey" , &Bind_SsAttribute::firstKey , return_value_policy<manage_new_object>() )
		.def("nextKey" , &Bind_SsAttribute::nextKey , return_value_policy<manage_new_object>() )
		.def("findRightKey" , &Bind_SsAttribute::findRightKey , return_value_policy<manage_new_object>() )
		.def("findLeftKey" , &Bind_SsAttribute::findLeftKey , return_value_policy<manage_new_object>() )
		;
    class_<Bind_Animation>("Animation")
        .def("init", &Bind_Animation::init)
		;

	class_<Bind_SsPart>("SsPart")
		.def("debug" , &Bind_SsPart::debug )
		.def("name" , &Bind_SsPart::name )
		.def("arrayIndex" , &Bind_SsPart::arrayIndex )
		.def("parentIndex" , &Bind_SsPart::parentIndex )
		.def("type" , &Bind_SsPart::type )
		.def("boundsType" , &Bind_SsPart::boundsType )
		.def("inheritType" , &Bind_SsPart::inheritType )
		.def("alphaBlendType" , &Bind_SsPart::alphaBlendType )
		.def("show" , &Bind_SsPart::show )
		.def("locked" , &Bind_SsPart::locked )
		.def("inheritRates" , &Bind_SsPart::inheritRates )
		;

	class_<Bind_SsPartAnime>("SsPartAnime")
		.def("name" , &Bind_SsPartAnime::name )
		.def("getAttribute" , &Bind_SsPartAnime::getAttribute , return_value_policy<manage_new_object>() )
		;

	class_<Bind_SsAnimeDecoder>("SsAnimeDecoder")
		.def("debug" , &Bind_SsAnimeDecoder::debug )
		.def("setFrame" , &Bind_SsAnimeDecoder::setFrame )
		.def("update" , &Bind_SsAnimeDecoder::update )
		.def("getPartNum" , &Bind_SsAnimeDecoder::getPartNum )
		.def("getPart" , &Bind_SsAnimeDecoder::getPart , return_value_policy<manage_new_object>() )
		.def("getPartAnime" , &Bind_SsAnimeDecoder::getPartAnime , return_value_policy<manage_new_object>() )
		;

	class_<Bind_SsAnimePack>("SsAnimePack")
		.def("debug" , &Bind_SsAnimePack::debug )
		//.def("get" , &Bind_SsAnimePack::get , return_value_policy<reference_existing_object>() )
		.def("getAnimeDecoderByName" , &Bind_SsAnimePack::getAnimeDecoderByName , return_value_policy<manage_new_object>() )
		.def("getAnimeDecoderByIndex" , &Bind_SsAnimePack::getAnimeDecoderByIndex , return_value_policy<manage_new_object>() )
			;

	class_<Bind_SsProject>("SsProject")
		.def("debug" , &Bind_SsProject::debug )
		.def("getAnimePackNum" , &Bind_SsProject::getAnimePackNum )
		.def("getCellMapNum" , &Bind_SsProject::getCellMapNum )
		.def("AnimePackAt" , &Bind_SsProject::AnimePackAt , return_value_policy<copy_const_reference>())
		;

	class_<SSXML>("SSXML")
		.def("Load" , &SSXML::Load )
		.def("GetPrj" ,  &SSXML::GetPrj , return_value_policy<copy_const_reference>() )
		;

};


int main(int argc, char* argv[])
{

	SSTextureFactory*	texfactory = new SSTextureFactory( new SSTextureBMP() );

    if(PyImport_AppendInittab("SpriteStudio", initSpriteStudio) == -1)
        puts("helloモジュールのセットアップに失敗");
    
    Py_Initialize();
    
    handle<> main_module(borrowed( PyImport_AddModule("__main__") ));
    handle<> main_namespace(borrowed( PyModule_GetDict(main_module.get()) ));
    
	//引数に与えられたPythonスクリプトファイルのロード
	std::string filename = "";
	filename = "testscript.py";	
	if ( argc == 2 )
	{
		filename = argv[1];
	}

	ifstream	f_in( 
		filename.c_str() , ios::binary );

	if ( f_in.fail()) {
		return 0;
	}
	ostringstream	str_out;
	str_out << f_in.rdbuf();
	std::string txt = str_out.str();
	f_in.close();

	const char	*s = txt.c_str();
	try{
		handle<>( PyRun_String( s, Py_file_input, main_namespace.get(), main_namespace.get()) );
	}catch(error_already_set const &)
	{
		// 何らかの方法で例外を処理する
		PyErr_Print();		
	}


    Py_Finalize();
    
	delete texfactory;
	fgetc(stdin);
    return 0;
}