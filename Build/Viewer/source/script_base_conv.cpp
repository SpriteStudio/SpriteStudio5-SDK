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



class Animation {
public:
    Animation(){ puts("construct Animation"); }
    char const* init(){
        return "Animation init";
    }
};


class SSXML
{
private:
	Animation	anime;
public:
	SSXML(){ puts("construct SSXML"); }
	char const*	Load( const char* name )
	{
		return name;
	}
	Animation	const& Get(){
		return anime;
	}
};


BOOST_PYTHON_MODULE(SpriteStudio)
{
	using namespace boost::python;

    class_<Animation>("Animation")
        .def("init", &Animation::init)
		;

	class_<SSXML>("SSXML")
		.def("Load" , &SSXML::Load )
		.def("Get" ,  &SSXML::Get , return_value_policy<copy_const_reference>() )
		;
		
};

int main(int argc, char* argv[])
{
    if(PyImport_AppendInittab("SpriteStudio", initSpriteStudio) == -1)
        puts("helloモジュールのセットアップに失敗");
    
    Py_Initialize();
    
    handle<> main_module(borrowed( PyImport_AddModule("__main__") ));
    handle<> main_namespace(borrowed( PyModule_GetDict(main_module.get()) ));
    
	//引数に与えられたPythonスクリプトファイルのロード
	std::string filename = argv[1];

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
    

	fgetc(stdin);
    return 0;
}