#include <stdlib.h>
#include <stdio.h>
#include <GL/glfw.h>
#include <emscripten/emscripten.h>
//#include "game.h"
 
int init_gl();
void do_frame();
void shutdown_gl();


#include "../../../common/Helper/OpenGL/SSTextureGL.h"
#include "../../../common/Drawer/ssplayer_shader_gl.h"
#include "../../../common/Drawer/ssplayer_render_gl.h"
#include "../../../common/Animator/ssplayer_animedecode.h"



#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glu32.lib")



SsAnimeDecoder*	m_player;
SsCellMapList*	m_cellmap;
SsProject*	m_proj;


int init_gl()
{
    const int width = 512,
             height = 512;
 
    if (glfwInit() != GL_TRUE) {
        printf("glfwInit() failed\n");
        return GL_FALSE;
    }
 
    if (glfwOpenWindow(width, height, 8, 8, 8, 8, 16, 0, GLFW_WINDOW) != GL_TRUE) {
        printf("glfwOpenWindow() failed\n");
        return GL_FALSE;
    }

	
	SSTextureFactory*	texfactory = new SSTextureFactory( new SSTextureGL() );//どこで解放しよう？
	SsCurrentRenderer::SetCurrentRender( new SsRenderGL() );

	m_player = new SsAnimeDecoder();
	m_cellmap = new SsCellMapList();

	m_proj = ssloader_sspj::Load( "input.sspj");
#if 0
	SsAnimePack* animepack = m_proj->getAnimePackList()[0]; 

	//アニメパックのパーツ構造を取得
	SsModel* model = &animepack->Model;

	//アニメパック内のアニメーションを選択
	SsAnimation* anime = animepack->animeList[0]; 

	//セルマップ情報を作成
	m_cellmap->set( m_proj , animepack );

	//パーツ構造　アニメーション　セルマップからアニメ再生情報を作成する
	if ( m_cellmap->size() == 0 )
	{
		DEBUG_PRINTF( "error : cellmap array size == 0" );
	}

	m_player->setAnimation( model , anime , m_cellmap , m_proj );

#endif

	return GL_TRUE;
}
 
void do_frame()
{

	//m_player->update(1.0f);
	//m_player->draw();
    //on_draw_frame();
    glfwSwapBuffers();
}
 
void shutdown_gl()
{
    glfwTerminate();
}
int main()
{
    if (init_gl() == GL_TRUE) {

        //on_surface_created();
        //on_surface_changed();
        emscripten_set_main_loop(do_frame, 0, 1);
    }
 
    shutdown_gl();
 
    return 0;
}


