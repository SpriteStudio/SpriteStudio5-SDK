CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(sssdk)

find_package(OpenGL)


ADD_LIBRARY(ssloader 
loader/tinyxml2/tinyxml2.cpp 
loader/babel/babel.cpp 
loader/babel/babel.h 
loader/ssloader.h
loader/ssloader_sspj.cpp 
loader/ssloader_ssae.cpp 
loader/ssloader_ssce.cpp 
loader/ssstring_uty.cpp 
loader/ssarchiver.cpp 
loader/ssarchiver.h 
loader/ssloader_sspj.h 
loader/ssloader_ssae.h 
loader/ssloader_ssce.h 
loader/sstypes.cpp 
loader/sstypes.h 
loader/ssvalue.cpp 
loader/ssvalue.h 
loader/ssInterpolation.h 
loader/ssInterpolation.cpp
loader/ssattribute.cpp 
loader/ssattribute.h 
loader/ssstring_uty.h)


ADD_EXECUTABLE(sssdk_test 
unittest/gtest_main.cc 
unittest/gtest-all.cc 
unittest/gtest/gtest.h 
unittest/unit_ssloader_sspj.cc 
unittest/unit_ssloader_ssce.cc
unittest/unit_ssloader_ssae.cc 
)

TARGET_LINK_LIBRARIES(sssdk_test ssloader)

ADD_EXECUTABLE(viewer_sample
viewer_sample/main.cpp 
viewer_sample/sampleScene.cpp 
viewer_sample/sampleScene.h 
viewer_sample/helper/stb_image.c 
viewer_sample/helper/stb_image.h 
viewer_sample/helper/tkTask.cpp 
viewer_sample/helper/tkTask.h 
viewer_sample/helper/tkTexture.cpp 
viewer_sample/helper/tkTexture.h 
viewer_sample/helper/tkObject.cpp 
viewer_sample/helper/tkObject.h 
viewer_sample/helper/tkScene.cpp 
viewer_sample/helper/tkScene.h 
viewer_sample/helper/glHelper.cpp 
viewer_sample/helper/glHelper.h 
viewer_sample/helper/WinMain.cpp 
viewer_sample/player/ssplayer_player.h 
viewer_sample/player/ssplayer_player.cpp 
viewer_sample/player/ssplayer_matrix.h 
viewer_sample/player/ssplayer_matrix.cpp
viewer_sample/player/ssplayer_shader.h 
viewer_sample/player/ssplayer_shader.cpp
viewer_sample/helper/XPFileOpenDlg.cpp
viewer_sample/helper/XPFileOpenDlg.h
viewer_sample/helper/OSXFileOpen.mm
viewer_sample/helper/OSXFileOpen.h
)

IF (WIN32)
    SET_TARGET_PROPERTIES (viewer_sample PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:windows")
    SET_TARGET_PROPERTIES (viewer_sample PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:windows")
    SET_TARGET_PROPERTIES (viewer_sample PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:windows")
    SET_TARGET_PROPERTIES (viewer_sample PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:windows")
ENDIF (WIN32)

IF(WIN32)
#	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows")
	SET(LINK_LIB "ssloader")
    ADD_DEFINITIONS(-_WIN32)
ENDIF(WIN32)

IF(APPLE)
	SET(SEARCH_DIR "/usr/local/include" )
	SET(SEARCH_LIB "/usr/local/lib" )
	SET(LINK_LIB "ssloader /usr/local/lib/libAntTweakBar.dylib /usr/local/lib/libglfw3.a -framework OpenGL -framework AppKit -framework IOKit" )
    ADD_DEFINITIONS(-APPLE)
ENDIF()


include_directories(${SEARCH_DIR})
link_directories(${SEARCH_LIB})
target_link_libraries(viewer_sample ${LINK_LIB})




