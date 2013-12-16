CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(sssdk)

find_package(OpenGL)


ADD_LIBRARY(ssloader 
../../Common/loader/tinyxml2/tinyxml2.cpp 
../../Common/loader/babel/babel.cpp 
../../Common/loader/babel/babel.h 
../../Common/loader/ssloader.h
../../Common/loader/ssloader_sspj.cpp 
../../Common/loader/ssloader_ssae.cpp 
../../Common/loader/ssloader_ssce.cpp 
../../Common/loader/ssstring_uty.cpp 
../../Common/loader/ssarchiver.cpp 
../../Common/loader/ssarchiver.h 
../../Common/loader/ssloader_sspj.h 
../../Common/loader/ssloader_ssae.h 
../../Common/loader/ssloader_ssce.h 
../../Common/loader/sstypes.cpp 
../../Common/loader/sstypes.h 
../../Common/loader/ssvalue.cpp 
../../Common/loader/ssvalue.h 
../../Common/loader/ssInterpolation.h 
../../Common/loader/ssInterpolation.cpp
../../Common/loader/ssattribute.cpp 
../../Common/loader/ssattribute.h 
../../Common/loader/ssstring_uty.h)


ADD_LIBRARY(ssAnimater
../../Common/Animator/ssplayer_render.cpp
../../Common/Animator/ssplayer_render.h
../../Common/Animator/ssplayer_matrix.cpp
../../Common/Animator/ssplayer_matrix.h
../../Common/Animator/ssplayer_animedecode.cpp
../../Common/Animator/ssplayer_animedecode.h)


ADD_LIBRARY(ssHelper
../../Common/Helper/Win/WinMain.cpp
../../Common/Helper/IsshTexture.cpp
../../Common/Helper/IsshTexture.h
../../Common/Helper/sshTextureBMP.cpp
../../Common/Helper/sshTextureBMP.h
../../Common/Helper/stb_image.c
../../Common/Helper/stb_image.h
../../Common/Helper/sshObject.h
../../Common/Helper/sshScene.cpp
../../Common/Helper/sshTask.cpp
../../Common/Helper/sshTask.h
../../Common/Helper/XPFileOpenDlg.h
../../Common/Helper/XPFileOpenDlg.cpp
../../Common/Helper/MacOS/OSXFileOpen.h
../../Common/Helper/MacOS/OSXFileOpen.mm)


ADD_EXECUTABLE(viewer_sample_gl
../../Common/Helper/OpenGL/SSTextureGL.cpp
../../Common/Helper/OpenGL/SSTextureGL.h
../../Common/Drawer/ssplayer_render_gl.h
../../Common/Drawer/ssplayer_render_gl.cpp
../../Common/Drawer/ssplayer_shader_gl.h
../../Common/Drawer/ssplayer_shader_gl.cpp
./source/main_gl.cpp
./source/myscene.cpp
./source/myscene.h
)


IF (WIN32)
    SET_TARGET_PROPERTIES (viewer_sample_gl PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:windows")
    SET_TARGET_PROPERTIES (viewer_sample_gl PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:windows")
    SET_TARGET_PROPERTIES (viewer_sample_gl PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:windows")
    SET_TARGET_PROPERTIES (viewer_sample_gl PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:windows")

#    SET_TARGET_PROPERTIES (viewer_sample_dx9 PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:windows")
#    SET_TARGET_PROPERTIES (viewer_sample_dx9 PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:windows")
#    SET_TARGET_PROPERTIES (viewer_sample_dx9 PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:windows")
#    SET_TARGET_PROPERTIES (viewer_sample_dx9 PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:windows")
ENDIF (WIN32)

IF(WIN32)
#	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows")
	SET(LINK_LIB1 "ssloader")
	SET(LINK_LIB2 "ssAnimater")
	SET(LINK_LIB3 "ssHelper")
	SET(LINK_LIB4 "")
    ADD_DEFINITIONS(-_WIN32)
ENDIF(WIN32)

IF(APPLE)
	SET(SEARCH_DIR "/usr/local/include" )
	SET(SEARCH_LIB "/usr/local/lib" )
	SET(LINK_LIB1 "ssloader")
	SET(LINK_LIB2 "ssAnimater")
	SET(LINK_LIB3 "ssHelper")	
	SET(LINK_LIB4 "/usr/local/lib/libAntTweakBar.dylib /usr/local/lib/libglfw3.a -framework OpenGL -framework AppKit -framework IOKit" )
    ADD_DEFINITIONS(-APPLE)
ENDIF()


include_directories(${SEARCH_DIR} ; ../../Common/Helper/ ; ../../Common/Drawer/ ; ../../Common/Animator)
link_directories(${SEARCH_LIB})
target_link_libraries(viewer_sample_gl ${LINK_LIB1} ${LINK_LIB2} ${LINK_LIB3} ${LINK_LIB4})

IF (WIN32)
#target_link_libraries(viewer_sample_dx9 ${LINK_LIB})
ENDIF()





