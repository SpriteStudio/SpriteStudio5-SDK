#if 1
#include <stdlib.h>
#include <stdio.h>
#include <GLFW/glfw3.h>
#include <emscripten/emscripten.h>
//#include "game.h"
 


#include "../../../common/Helper/OpenGL/SSTextureGL.h"
#include "../../../common/Drawer/ssplayer_shader_gl.h"
#include "../../../common/Drawer/ssplayer_render_gl.h"
#include "../../../common/Animator/ssplayer_animedecode.h"



SsAnimeDecoder*	m_player;
SsCellMapList*	m_cellmap;
SsProject*	m_proj;
GLFWwindow *window;





///サンプル用のシーンクラス　
class SampleScene : public tkScene
{
public:
	struct AnimePackSelecterType;


private:

	SsAnimeDecoder*	m_player;
	SsCellMapList*	m_cellmap;


	SsProject*	m_proj;
	int			m_select_animepack_index;
	int			m_select_anime_index;

	SsVector2	m_animeOffsetpos;
	int			m_nowPlayFrame;
	double		m_nowPlayFrameD;

	bool		m_isAnimeAutoPlay;

	int			m_InfoAnimeEndFrame;
	int			m_InfoAnimeFps;

	float		m_Zoom;
	float		m_Speed;

public:
	SampleScene() : m_isAnimeAutoPlay (true) {}
	virtual ~SampleScene()
	{
		destroy();
	}

	virtual void	draw()
	{
		m_player->draw();	
	}
	virtual void	update(double delta)
	{
		float frameDelta = 0;
		if ( m_player )
		{
			m_InfoAnimeFps = m_player->getAnimeFPS();
			m_InfoAnimeEndFrame = m_player->getAnimeEndFrame();
		}
		if ( m_isAnimeAutoPlay )
		{
			double anime_fps = (double)m_player->getAnimeFPS();
			double frameper_sec = (1.0 / anime_fps) ;
			frameDelta = (delta*m_Speed / frameper_sec );

			m_nowPlayFrameD+= frameDelta;
			m_nowPlayFrame = (int)m_nowPlayFrameD;

			if ( m_player->getAnimeEndFrame() < (int)m_nowPlayFrame )
			{
				m_nowPlayFrame = 0;
				m_nowPlayFrameD = 0;
				m_player->restart();
			}
		}
		static float backframe = 0;
	
		m_player->setPlayFrame( (float)m_nowPlayFrame );
		m_player->update(frameDelta);


		backframe = m_nowPlayFrame;	
	
	}
	virtual void	init()
	{
		m_select_animepack_index = 0;
		m_select_anime_index = 0;
		m_animeOffsetpos.x = 0;
		m_animeOffsetpos.y = 0;
		m_nowPlayFrame = 0;
		m_proj = 0;

		m_nowPlayFrame = 0;
		m_nowPlayFrameD = 0;

		m_InfoAnimeEndFrame = 0;
		m_InfoAnimeFps = 0;

		m_Zoom = 1.0f;
		m_Speed = 1.0f;

		m_player = new SsAnimeDecoder();
		m_cellmap = new SsCellMapList();	



	}
	virtual void	destroy()
	{
		delete m_player;
		m_player = 0;
	}

	void	Load( const char* name )
	{
		//ロード
		m_proj = ssloader_sspj::Load( name );
		if ( m_proj )
		{
			SsAnimePack* animepack = m_proj->getAnimePackList()[0]; 

			//アニメパックのパーツ構造を取得
			SsModel* model = &animepack->Model;

			//アニメパック内のアニメーションを選択
			SsAnimation* anime = animepack->animeList[0]; 

			printf( "set animepack = %s\n" , animepack->name.c_str() );
			printf( "set anime = %s\n" , anime->name.c_str() );

			//セルマップ情報を作成
			m_cellmap->set( m_proj , animepack );
			//パーツ構造　アニメーション　セルマップからアニメ再生情報を作成する
			if ( m_cellmap->size() == 0 )
			{
				DEBUG_PRINTF( "error : cellmap array size == 0" );
			}
			m_player->setAnimation( model , anime , m_cellmap , m_proj );
		}else{
			DEBUG_PRINTF( "error : sspj load error" );
		}		
	}

	void	AnimePlay(){ m_isAnimeAutoPlay = true; }
	void	AnimePause(){ m_isAnimeAutoPlay = false; }
	void	AnimeReset(){ 
		m_isAnimeAutoPlay = false; 
		m_nowPlayFrame = 0;
		m_nowPlayFrameD = 0;
	}




};








/* =====================================================================================
	update
===================================================================================== */
static void	do_frame()
{
	static double back = glfwGetTime();
	double t = glfwGetTime();
	double delta = t - back;
	back = t;

	task_manager_singleton::getinst()->exec_resist_tasks(delta);

	glClear( GL_COLOR_BUFFER_BIT );
	glClearColor( 0 , 0 , 0 , 1.0f );

	task_manager_singleton::getinst()->draw_resist_tasks();

	glfwSwapBuffers(window);

}


int init_gl()
{
    const int width = 1024,
             height = 720;
 
    if (glfwInit() != GL_TRUE) {
        printf("glfwInit() failed\n");
        return GL_FALSE;
    }
	
	if ( !( window = glfwCreateWindow(width, height, "Emscripten", NULL, NULL)) )
	{
        printf("glfwOpenWindow() failed\n");
        return GL_FALSE;
    }

	
	new task_manager_singleton();
	SampleScene* sample = new SampleScene();
	task_resist_task( 0 , sample );

	
	SSTextureFactory*	texfactory = new SSTextureFactory( new SSTextureGL() );//どこで解放しよう？
	SsCurrentRenderer::SetCurrentRender( new SsRenderGL() );

	sample->Load( "input.sspj");



	//ビューアなので最初の1回でいいかも
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    int phyWidth,phyHeight;
    glfwGetMonitorPhysicalSize(monitor, &phyWidth , &phyHeight);

//	int width , height;
	int fbwidth , fbheight;
//    glfwGetWindowSize( window, &width, &height);
	glfwGetFramebufferSize( window , &fbwidth , &fbheight );
    
	glMatrixMode(GL_PROJECTION);
	glViewport( 0 , 0 , fbwidth , fbheight );
	glLoadIdentity();
	glOrtho(-width / 2, width / 2, -height / 2, height / 2, -2048, 2048);

	glScaled(0.5f, 0.5f, 1.f);


	return GL_TRUE;
}
 void renderGame()
{
	glClear(GL_COLOR_BUFFER_BIT);

	float ratio;
	int width, height;

	glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef((float) glfwGetTime() * 50.f, 0.f, 0.f, 1.f);
    glBegin(GL_TRIANGLES);
    glColor3f(1.f, 0.f, 0.f);
    glVertex3f(-0.6f, -0.4f, 0.f);
    glColor3f(0.f, 1.f, 0.f);
    glVertex3f(0.6f, -0.4f, 0.f);
    glColor3f(0.f, 0.f, 1.f);
    glVertex3f(0.f, 0.6f, 0.f);
    glEnd();

	glfwSwapBuffers(window);
	glfwPollEvents();
}

#if 0
void do_frame()
{

	//update();

	static int m_InfoAnimeFps , m_InfoAnimeEndFrame;
	static double back = glfwGetTime();
	static int			m_nowPlayFrame;
	static double		m_nowPlayFrameD;
	static float m_Speed = 1.0f;
	static float backframe = 0;
	static bool m_isAnimeAutoPlay = true;

	double t = glfwGetTime();
	double delta = t - back;

//	printf( "do_frame" );
//	renderGame();

	//====== Update =========================
	float frameDelta = 0;
	if ( m_player )
	{
		m_InfoAnimeFps = m_player->getAnimeFPS();
		m_InfoAnimeEndFrame = m_player->getAnimeEndFrame();
	}

	if ( m_isAnimeAutoPlay )
	{
		double anime_fps = (double)m_player->getAnimeFPS();
		double frameper_sec = (1.0 / anime_fps) ;
		frameDelta = (delta*m_Speed / frameper_sec );

		m_nowPlayFrameD+= frameDelta;
		m_nowPlayFrame = (int)m_nowPlayFrameD;

		if ( m_player->getAnimeEndFrame() < (int)m_nowPlayFrame )
		{
			m_nowPlayFrame = 0;
			m_nowPlayFrameD = 0;
			m_player->restart();
		}
	}
	
	m_player->setPlayFrame( (float)m_nowPlayFrame );
	m_player->update(frameDelta);


	backframe = m_nowPlayFrame;


	//====== Draw =========================
	glClearColor(0.0, 0.0, 0.0, 1.0);


    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    int phyWidth,phyHeight;
    glfwGetMonitorPhysicalSize(monitor, &phyWidth , &phyHeight);

	int width , height;
	int fbwidth , fbheight;
    glfwGetWindowSize( window, &width, &height);
	glfwGetFramebufferSize( window , &fbwidth , &fbheight );
    
	glMatrixMode(GL_PROJECTION);
	glViewport( 0 , 0 , fbwidth , fbheight );
	glLoadIdentity();
	glOrtho(-width / 2, width / 2, -height / 2, height / 2, -2048, 2048);

	glScaled(0.5f, 0.5f, 1.f);

	m_player->draw();

    glfwSwapBuffers(window);
}
#endif
 
void shutdown_gl()
{
    glfwTerminate();
}
int main()
{
    if (init_gl() == GL_TRUE) {
		printf( "init_gl = OK " );
        emscripten_set_main_loop(do_frame, 60, 1);
    }

	 printf( "shutdown_gl" );
    shutdown_gl();
 
    return 0;
}


#else
#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <emscripten/emscripten.h>

GLFWwindow *window;

void errorCallback(int error, const char *description) {
    fputs(description, stderr);
}

void renderGame()
{
	glClear(GL_COLOR_BUFFER_BIT);

	float ratio;
	int width, height;

	glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef((float) glfwGetTime() * 50.f, 0.f, 0.f, 1.f);
    glBegin(GL_TRIANGLES);
    glColor3f(1.f, 0.f, 0.f);
    glVertex3f(-0.6f, -0.4f, 0.f);
    glColor3f(0.f, 1.f, 0.f);
    glVertex3f(0.6f, -0.4f, 0.f);
    glColor3f(0.f, 0.f, 1.f);
    glVertex3f(0.f, 0.6f, 0.f);
    glEnd();

	glfwSwapBuffers(window);
	glfwPollEvents();
}

int main(int argc, char **argv) {
	glfwSetErrorCallback(errorCallback);

	if (!glfwInit()) {
		fputs("Failed to initialize GLFW3!", stderr);
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(640, 480, "Emscripten", NULL, NULL);

	if (!window) {
		fputs("Failed to create GLFW3 window!", stderr);
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	/* Initialize main loop which is the same as:
	 * window.requestAnimationFrame(renderGame)
	 * in this case.
	 */
	emscripten_set_main_loop(renderGame, 0, 0);
}
#endif