//

#define BOOST_PYTHON_STATIC_LIB
#include    <stdio.h>
#include    <Python.h>
#include    <boost/python.hpp>
#include <boost/python/numeric.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>


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

	//numpyモジュールを使用する
	numeric::array::set_module_and_type("numpy", "ndarray");

	class_<std::vector<float> > ("PyVecFloat")
	    .def(boost::python::vector_indexing_suite<std::vector<float> > () )
		;

	class_<Bind_SsTexture>("Bind_SsTexture")
		.add_property("width", &Bind_SsTexture::getWidth)
		.add_property("height", &Bind_SsTexture::getHeight)
		.add_property("filename", &Bind_SsTexture::getFilename)
		;

	class_<Bind_SsCellValue>("Bind_SsCellValue")
		.add_property("wrapMode", &Bind_SsCellValue::wrapMode)
		.add_property("filterMode", &Bind_SsCellValue::filterMode)
		.def("getUvs" , &Bind_SsCellValue::getuvs )
		.def("getCell" , &Bind_SsCellValue::cell , return_value_policy<manage_new_object>() )
		.def("getTexture" , &Bind_SsCellValue::texture , return_value_policy<manage_new_object>() )
		;

	class_<Bind_SsPartState>("Bind_SsPartState")
		.add_property("index", &Bind_SsPartState::index)
		.add_property("vertices", &Bind_SsPartState::vertices)
		.add_property("colors", &Bind_SsPartState::colors)
		.add_property("uvs", &Bind_SsPartState::uvs)
		.add_property("matrix", &Bind_SsPartState::matrix)
		.add_property("position", &Bind_SsPartState::position)
		.add_property("rotation", &Bind_SsPartState::rotation)
		.add_property("scale", &Bind_SsPartState::scale)
		.add_property("alpha", &Bind_SsPartState::alpha)
		.add_property("prio", &Bind_SsPartState::prio)
		.add_property("hFlip", &Bind_SsPartState::hFlip)
		.add_property("vFlip", &Bind_SsPartState::vFlip)
		.add_property("hide", &Bind_SsPartState::hide)
		.add_property("pivotOffset", &Bind_SsPartState::pivotOffset)
		.add_property("anchor", &Bind_SsPartState::anchor)
		.add_property("size", &Bind_SsPartState::size)
		.add_property("imageFlipH", &Bind_SsPartState::imageFlipH)
		.add_property("imageFlipV", &Bind_SsPartState::imageFlipV)
		.add_property("uvTranslate", &Bind_SsPartState::uvTranslate)
		.add_property("uvRotation", &Bind_SsPartState::uvRotation)
		.add_property("uvScale", &Bind_SsPartState::uvScale)
		.add_property("boundingRadius", &Bind_SsPartState::boundingRadius)
		.add_property("noCells", &Bind_SsPartState::noCells)
		.add_property("is_color_blend", &Bind_SsPartState::is_color_blend)
		.add_property("is_vertex_transform", &Bind_SsPartState::is_vertex_transform)
		.add_property("alphaBlendType", &Bind_SsPartState::alphaBlendType)
		.add_property("colorValue", &Bind_SsPartState::colorValue)
		.add_property("vertexValue", &Bind_SsPartState::vertexValue)
		.def("getCellValue" , &Bind_SsPartState::cellValue , return_value_policy<manage_new_object>() )
		;

	class_<Bind_Cell>("Bind_Cell")
		.def("debug" , &Bind_Cell::debug )
		.add_property("name", &Bind_Cell::name)
		.add_property("pos", &Bind_Cell::pos)
		.add_property("size", &Bind_Cell::size)
		.add_property("pivot", &Bind_Cell::pivot)
		.add_property("rotated", &Bind_Cell::rotated)
		.def("isNull" , &Bind_Cell::isNull )
		;

	class_<Bind_Cellmap>("Bind_Cellmap")
		.add_property("name", &Bind_Cellmap::name)
		.add_property("imagePath", &Bind_Cellmap::imagePath)
		.add_property("pixelSize", &Bind_Cellmap::pixelSize)
		.add_property("overrideTexSettings", &Bind_Cellmap::overrideTexSettings)
		.add_property("wrapMode", &Bind_Cellmap::wrapMode)
		.add_property("filterMode", &Bind_Cellmap::filterMode)
		.add_property("length", &Bind_Cellmap::getCellNum)
		.def("getCell" , &Bind_Cellmap::getCell , return_value_policy<manage_new_object>() )
		;

	class_<SsRefCell>("SsRefCell")
		.def_readonly("mapid", &SsRefCell::mapid)
		.def_readonly("name", &SsRefCell::name)
		;

	class_<SsUserDataAnime>("SsUserDataAnime")
		.def_readonly("useInteger", &SsUserDataAnime::useInteger)
		.def_readonly("usePoint", &SsUserDataAnime::usePoint)
		.def_readonly("useRect", &SsUserDataAnime::useRect)
		.def_readonly("useString", &SsUserDataAnime::useString)
		.def_readonly("integer", &SsUserDataAnime::integer)
		.def_readonly("point", &SsUserDataAnime::point)
		.def_readonly("rect", &SsUserDataAnime::rect)
		.def_readonly("string", &SsUserDataAnime::string)
		;


	//基本的にリードオンリーで定義しておく
	class_<SsColor>("SsColor")
		.def("toARGB" , &SsColor::toARGB )
		.def_readonly("a", &SsColor::a)
		.def_readonly("r", &SsColor::r)
		.def_readonly("g", &SsColor::g)
		.def_readonly("b", &SsColor::b)
		;
	
	class_<SsPoint2>("SsPoint2")
		.def_readonly("x", &SsPoint2::x)
		.def_readonly("y", &SsPoint2::y)
		;

	class_<SsVector3>("SsVector3")
		.def_readonly("x", &SsVector3::x)
		.def_readonly("y", &SsVector3::y)
		.def_readonly("z", &SsVector3::z)
		;

	class_<SsIRect>("SsIRect")
		.def_readonly("x", &SsIRect::x)
		.def_readonly("y", &SsIRect::y)
		.def_readonly("w", &SsIRect::w)
		.def_readonly("h", &SsIRect::h)
		;

	class_<SsColorBlendValue>("SsColorBlendValue")
		.def_readonly("rgba", &SsColorBlendValue::rgba)
		.def_readonly("rate", &SsColorBlendValue::rate)
		;	
	
	class_<SsVertexAnime>("SsVertexAnime")
		.def("offsets" , &SsVertexAnime::getOffsets , return_value_policy<reference_existing_object>())
		;

	class_<SsColorAnime>("SsColorAnime")
		.add_property("target", &SsColorAnime::getTargetToInt)
		.add_property("blendType", &SsColorAnime::getBlendTypeToInt)
		.def_readonly("color", &SsColorAnime::color)
		.def("colors" , &SsColorAnime::getColors , return_value_policy<reference_existing_object>())
		;

	//read onlyのプロパティとしてインポート
	class_<SsCurve>("SsCurve")
		.def_readonly("startTime", &SsCurve::startTime)
		.def_readonly("startValue", &SsCurve::startValue)
		.def_readonly("endTime", &SsCurve::endTime)
		.def_readonly("endValue", &SsCurve::endValue)
		.def_readonly("startKeyTime", &SsCurve::startKeyTime)
		.def_readonly("endKeyTime", &SsCurve::endKeyTime)
		;


    class_<Bind_SsKeyValue>("Bind_SsKeyValue")
		.def("getType" , &Bind_SsKeyValue::getType )
		.def("toInt" , &Bind_SsKeyValue::toInt )
		.def("toFloat" , &Bind_SsKeyValue::toFloat )
		.def("toBool" , &Bind_SsKeyValue::toBool )
		;

    class_<Bind_SsKeyframe>("SsKeyframe")
		.def("debug" , &Bind_SsKeyframe::debug )
		.def("time" , &Bind_SsKeyframe::time )
		.def("isNull" , &Bind_SsKeyframe::isNull )
		.def("interpolationType" , &Bind_SsKeyframe::InterpolationType )
		.def("getCurveParam" , &Bind_SsKeyframe::getCurveParam , return_value_policy<reference_existing_object>())
		.def("getValue" , &Bind_SsKeyframe::getValue , return_value_policy<reference_existing_object>())
		.def("getColorAnime" , &Bind_SsKeyframe::getColorAnime , return_value_policy<manage_new_object>() )
		.def("getVertexAnime" , &Bind_SsKeyframe::getVertexAnime , return_value_policy<manage_new_object>() )
		.def("getRefCell" , &Bind_SsKeyframe::getRefCell , return_value_policy<manage_new_object>() )
		.def("getUserDataAnime" , &Bind_SsKeyframe::getUserDataAnime , return_value_policy<manage_new_object>() )
		;
	

    class_<Bind_SsAttribute>("SsAttribute")
		.def("debug" , &Bind_SsAttribute::debug )
		.def("isNull" , &Bind_SsAttribute::isNull )
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
		.def("getFrameLength" , &Bind_SsAnimeDecoder::getFrameLength )
		.def("getAnimeFPS" , &Bind_SsAnimeDecoder::getAnimeFPS )
		.def("getPartState" , &Bind_SsAnimeDecoder::getPartState , return_value_policy<manage_new_object>() )
		;

	class_<Bind_SsAnimePack>("SsAnimePack")
		.def("debug" , &Bind_SsAnimePack::debug )
		//.def("get" , &Bind_SsAnimePack::get , return_value_policy<reference_existing_object>() )
		.def("getAnimeDecoderByName" , &Bind_SsAnimePack::getAnimeDecoderByName , return_value_policy<manage_new_object>() )
		.def("getAnimeDecoderByIndex" , &Bind_SsAnimePack::getAnimeDecoderByIndex , return_value_policy<manage_new_object>() )
			;

	class_<Bind_SsProjectSetting>("Bind_SsProjectSetting")
		.add_property("animeBaseDirectory", &Bind_SsProjectSetting::animeBaseDirectory)
		.add_property("cellMapBaseDirectory", &Bind_SsProjectSetting::cellMapBaseDirectory)
		.add_property("imageBaseDirectory", &Bind_SsProjectSetting::imageBaseDirectory)
		.add_property("exportBaseDirectory", &Bind_SsProjectSetting::exportBaseDirectory)
		.add_property("queryExportBaseDirectory", &Bind_SsProjectSetting::queryExportBaseDirectory)
		.add_property("wrapMode", &Bind_SsProjectSetting::wrapMode)
		.add_property("filterMode", &Bind_SsProjectSetting::filterMode)
		;

	class_<Bind_SsProject>("SsProject")
		.def("debug" , &Bind_SsProject::debug )
		.def("getAnimePackNum" , &Bind_SsProject::getAnimePackNum )
		.def("getCellMapNum" , &Bind_SsProject::getCellMapNum )
		.def("AnimePackAt" , &Bind_SsProject::AnimePackAt , return_value_policy<copy_const_reference>())
		.def("getCellMapAt" , &Bind_SsProject::getCellMapAt , return_value_policy<manage_new_object>() )
		.def("getCellMapFromName" , &Bind_SsProject::getCellMapFromName , return_value_policy<manage_new_object>() )
//		.add_property("setting", &Bind_SsProject::settings)
		.def("getSettings" , &Bind_SsProject::settings , return_value_policy<reference_existing_object>())
	;

	class_<SSXML>("SSXML")
		.def("Load" , &SSXML::Load )
		.def("GetPrj" ,  &SSXML::GetPrj , return_value_policy<copy_const_reference>() )
		;

};


int main(int argc, char* argv[])
{

	SSTextureFactory*	texfactory = new SSTextureFactory( new SSTextureBMP() );

    if(PyImport_AppendInittab( "SpriteStudio", initSpriteStudio) == -1)
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


	Py_Main(  argc , argv );
/*
	try{
		handle<>( PyRun_String( s, Py_file_input, main_namespace.get(), main_namespace.get()) );
	}catch(error_already_set const &)
	{
		// 何らかの方法で例外を処理する
		PyErr_Print();		
	}
*/

    Py_Finalize();
    
	delete texfactory;
	fgetc(stdin);
    return 0;
}