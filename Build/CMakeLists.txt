CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(sssdk)

IF (WIN32)
#SET(BOOST_ROOT "C:/boost-1_56_0")
SET(BOOST_ROOT "C:/Boost/include/boost-1_56/")
SET(BOOST_PYTHON_LIB "C:/Boost/lib" )
SET(PYTHON_IHNCLUDE "C:/Python27/include")

SET(PYTHON_LIB "C:/Python27/libs")
ENDIF(WIN32)

IF(APPLE)
SET(PYTHON_LIB "/usr/local/lib")
ENDIF(APPLE)

link_directories(${SEARCH_LIB} ${BOOST_PYTHON_LIB} ${PYTHON_LIB}  )


ADD_LIBRARY(ssloader 
../Common/loader/tinyxml2/tinyxml2.cpp 
../Common/loader/babel/babel.cpp 
../Common/loader/babel/babel.h 
../Common/loader/ssloader.h
../Common/loader/ssloader_sspj.cpp 
../Common/loader/ssloader_ssae.cpp 
../Common/loader/ssloader_ssce.cpp 
../Common/loader/ssloader_ssee.cpp 
../Common/loader/effect/SsEffectBehavior.cpp
../Common/loader/effect/SsEffectBehavior.h
../Common/loader/effect/SsEffectElement.h
../Common/loader/ssstring_uty.cpp 
../Common/loader/ssarchiver.cpp 
../Common/loader/ssarchiver.h 
../Common/loader/ssloader_sspj.h 
../Common/loader/ssloader_ssae.h 
../Common/loader/ssloader_ssce.h 
../Common/loader/ssloader_ssee.h 
../Common/loader/sstypes.cpp 
../Common/loader/sstypes.h 
../Common/loader/ssvalue.cpp 
../Common/loader/ssvalue.h 
../Common/loader/ssInterpolation.h 
../Common/loader/ssInterpolation.cpp
../Common/loader/ssattribute.cpp 
../Common/loader/ssattribute.h 
../Common/loader/ssstring_uty.h)


ADD_LIBRARY(ssAnimater
../Common/Animator/ssplayer_types.h
../Common/Animator/ssplayer_cellmap.h
../Common/Animator/ssplayer_cellmap.cpp
../Common/Animator/ssplayer_PartState.h
../Common/Animator/ssplayer_PartState.cpp
../Common/Animator/ssplayer_render.cpp
../Common/Animator/ssplayer_render.h
../Common/Animator/ssplayer_matrix.cpp
../Common/Animator/ssplayer_matrix.h
../Common/Animator/ssplayer_effect.cpp
../Common/Animator/ssplayer_effect.h
../Common/Animator/MersenneTwister.h
../Common/Animator/ssplayer_animedecode.cpp
../Common/Animator/ssplayer_animedecode.h)


ADD_LIBRARY(ssHelper
../Common/Helper/Win/WinMain.cpp
../Common/Helper/IsshTexture.cpp
../Common/Helper/IsshTexture.h
../Common/Helper/sshTextureBMP.cpp
../Common/Helper/sshTextureBMP.h
../Common/Helper/stb_image.c
../Common/Helper/stb_image.h
../Common/Helper/sshObject.h
../Common/Helper/sshScene.cpp
../Common/Helper/sshTask.cpp
../Common/Helper/sshTask.h
../Common/Helper/XPFileOpenDlg.h
../Common/Helper/XPFileOpenDlg.cpp
../Common/Helper/DebugPrint.h
../Common/Helper/DebugPrint.cpp
../Common/Helper/MacOS/OSXFileOpen.h
../Common/Helper/MacOS/OSXFileOpen.mm)


ADD_EXECUTABLE(viewer_sample_gl
../Common/Helper/OpenGL/SSTextureGL.cpp
../Common/Helper/OpenGL/SSTextureGL.h
../Common/Drawer/ssplayer_render_gl.h
../Common/Drawer/ssplayer_render_gl.cpp
../Common/Drawer/ssplayer_shader_gl.h
../Common/Drawer/ssplayer_shader_gl.cpp
./Viewer/source/main_gl.cpp
./Viewer/source/myscene.cpp
./Viewer/source/myscene.h
)

ADD_EXECUTABLE(viewer_sample_dx9
../Common/Helper/DirectX/d3dsimple.cpp
../Common/Helper/DirectX/d3dsimple.h
../Common/Helper/DirectX/SSTextureDX9.cpp
../Common/Helper/DirectX/SSTextureDX9.h
../Common/Drawer/ssplayer_render_dx9.h
../Common/Drawer/ssplayer_render_dx9.cpp
./Viewer/source/main_dx9.cpp
./Viewer/source/myscene.cpp
./Viewer/source/myscene.h
)

#IF(WIN32)
#ADD_EXECUTABLE(scriptbase_converter
#./PythonBinder/script_base_conv.cpp
#./PythonBinder/bind_SSXML.cpp
#./PythonBinder/bind_SSXML.h
#./PythonBinder/bind_Animation.cpp
#./PythonBinder/bind_Animation.h
#./PythonBinder/bind_SsPart.cpp
#./PythonBinder/bind_SsPart.h
#./PythonBinder/bind_SsPartAnime.cpp
#./PythonBinder/bind_SsPartAnime.h
#./PythonBinder/bind_SsAnimeDecoder.cpp
#./PythonBinder/bind_SsAnimeDecoder.h
#./PythonBinder/bind_AnimePack.cpp
#./PythonBinder/bind_AnimePack.h
#./PythonBinder/bind_SsAttribute.cpp
#./PythonBinder/bind_SsAttribute.h
#./PythonBinder/bind_SsKeyframe.cpp
#./PythonBinder/bind_SsKeyframe.h
#./PythonBinder/package_SpriteStudio.cpp
#./PythonBinder/package_SpriteStudio.h
#./PythonBinder/bind_Cellmap.cpp
#./PythonBinder/bind_Cellmap.h
#)
#ENDIF(WIN32)


ADD_EXECUTABLE(Ss5Converter
./Converter/main.cpp
./Converter/BinaryDataWriter.cpp
./Converter/BinaryDataWriter.h
./Converter/FileUtil.cpp
./Converter/FileUtil.h
./Converter/Lump.cpp
./Converter/Lump.h
./Converter/LumpExporter.cpp
./Converter/LumpExporter.h
./Converter/SsPlayerConverter.h
)


IF (WIN32)
    SET_TARGET_PROPERTIES (viewer_sample_gl PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:windows")
    SET_TARGET_PROPERTIES (viewer_sample_gl PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:windows")
    SET_TARGET_PROPERTIES (viewer_sample_gl PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:windows")
    SET_TARGET_PROPERTIES (viewer_sample_gl PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:windows")

    SET_TARGET_PROPERTIES (viewer_sample_dx9 PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:windows")
    SET_TARGET_PROPERTIES (viewer_sample_dx9 PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:windows")
    SET_TARGET_PROPERTIES (viewer_sample_dx9 PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:windows")
    SET_TARGET_PROPERTIES (viewer_sample_dx9 PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:windows")


ENDIF (WIN32)

IF(WIN32)
#	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows")
	SET(LINK_LIB1 "ssloader")
	SET(LINK_LIB2 "ssAnimater")
	SET(LINK_LIB3 "ssHelper")
	SET(LINK_LIB4 "")
    ADD_DEFINITIONS(-_WIN32)
	SET(SEARCH_LIB "" )
ENDIF(WIN32)

IF(APPLE)
	SET(SEARCH_DIR "/usr/local/include" )
	SET(SEARCH_LIB "/usr/local/lib" )
	SET(LINK_LIB1 "ssloader")
	SET(LINK_LIB2 "ssAnimater")
	SET(LINK_LIB3 "ssHelper")	
	SET(LINK_LIB4 "libAntTweakBar.dylib libglfw3.a -framework OpenGL -framework AppKit -framework IOKit" )
    ADD_DEFINITIONS(-APPLE)
ENDIF()

#link_directories( SEARCH_LIB )
include_directories(;${SEARCH_DIR} ; ${PYTHON_IHNCLUDE} ; ${BOOST_ROOT} ; ../Common/Loader/ ;../Common/Helper/ ; ../Common/Drawer/ ; ../Common/Animator)
target_link_libraries(viewer_sample_gl ${LINK_LIB1} ${LINK_LIB2} ${LINK_LIB3} ${LINK_LIB4} )

#IF (WIN32)
#target_link_libraries(scriptbase_converter ${LINK_LIB1} ${LINK_LIB2} ${LINK_LIB3} ${LINK_LIB4} )
#ENDIF(WIN32)

IF (WIN32)
target_link_libraries(viewer_sample_dx9 ${LINK_LIB1} ${LINK_LIB2} ${LINK_LIB3} ${LINK_LIB4})
ENDIF()


target_link_libraries(Ss5Converter ${LINK_LIB1} ${LINK_LIB2} ${LINK_LIB3} )


