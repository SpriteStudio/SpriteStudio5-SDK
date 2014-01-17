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

    class_<Animation>("Animation")
        .def("init", &Animation::init)
		;

	class_<Bind_SsAnimeDecoder>("Bind_SsAnimeDecoder")
		.def("debug" , &Bind_SsAnimeDecoder::debug )
		;

	class_<Bind_SsAnimePack>("Bind_SsAnimePack")
		.def("debug" , &Bind_SsAnimePack::debug )
		//.def("get" , &Bind_SsAnimePack::get , return_value_policy<reference_existing_object>() )
		.def("getAnimeDecoderByName" , &Bind_SsAnimePack::getAnimeDecoderByName , return_value_policy<manage_new_object>() )
		.def("getAnimeDecoderByIndex" , &Bind_SsAnimePack::getAnimeDecoderByIndex , return_value_policy<manage_new_object>() )
			;

	class_<Bind_SsProject>("Bind_SsProject")
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

    handle<>( PyRun_String( s, Py_file_input, main_namespace.get(), main_namespace.get()) );
    
    Py_Finalize();
    
	delete texfactory;
	fgetc(stdin);
    return 0;
}