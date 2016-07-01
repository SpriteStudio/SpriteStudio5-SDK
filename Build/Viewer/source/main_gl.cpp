#ifdef _WIN32
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <AntTweakBar.h>

#include "ssHelper.h"
#include "myscene.h"

#include "OpenGL/SSTextureGL.h"


#include "ssplayer_shader_gl.h"
//#include "ssplayer_render.h"
#include "ssplayer_render_gl.h"


#ifdef _WIN32
#pragma comment(lib, "glew32.lib")

#ifdef _DEBUG
#pragma comment(lib, "glfw3d.lib")
#else
#pragma comment(lib, "glfw3.lib")
#endif
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "AntTweakBar.lib")
#else
//#define NULL (0)
#endif


#define USING_OPENGLES (0)

GLFWvidmode mode;   // GLFW video mode
TwBar *g_twbar;         // Pointer to a tweak bar
    
float bgColor[] = { 0.1f, 0.2f, 0.4f };         // Background color 

float displayscale = 1.0f;


/* =====================================================================================
	update
===================================================================================== */
static void	update()
{

	static double back = glfwGetTime();
	double t = glfwGetTime();
	double delta = t - back;

	back = t;

	task_manager_singleton::getinst()->exec_resist_tasks(delta);

	glClear( GL_COLOR_BUFFER_BIT );
	glClearColor( bgColor[0] , bgColor[1] , bgColor[2] , 1.0f );

	task_manager_singleton::getinst()->draw_resist_tasks();

}

/* =====================================================================================
	initialize
===================================================================================== */
static void	initialize()
{
//	SsRender::shaderInit();
	SsCurrentRenderer::SetCurrentRender( new SsRenderGL() );

	SampleScene* sample = new SampleScene();
	task_resist_task( 0 , sample );


}

static void	update_view( GLFWwindow* window )
{
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
	
	TwWindowSize(fbwidth, fbheight);
    
    displayscale = (float)fbwidth / (float)width;
    
}



void	framebuffer_size_callback( GLFWwindow* window , int width , int height )
{

	update_view( window );


}

//typedef void (* GLFWmousebuttonfun)(GLFWwindow*,int,int,int);
void MouseButtonCB( GLFWwindow*,int button ,int action ,int mods)
{
	TwEventMouseButtonGLFW( button , action );
}

//typedef void (* GLFWcursorposfun)(GLFWwindow*,double,double);
void	MousePosCB(GLFWwindow*,double x ,double y)
{
	TwEventMousePosGLFW( (int)(x*displayscale) , (int)(y*displayscale) );
}

//typedef void (* GLFWkeyfun)(GLFWwindow*,int,int,int,int);
void	KeyFunCB( GLFWwindow* window,int key,int scancode,int action,int mods)
{
	TwEventKeyGLFW( key , action );
	TwEventCharGLFW( key  , action );

}

//typedef void (* GLFWcharfun)(GLFWwindow*,unsigned int);
void	CharFunCB( GLFWwindow* window,unsigned int character)
{
//	TwEventCharGLFW( character );

}

//typedef void (* GLFWscrollfun)(GLFWwindow*,double,double);

void	MouseScrollCB(  GLFWwindow* window, double x , double y )
{
	TwEventMouseWheelGLFW( (int)y );
}

GLFWwindow* window;


void    GlContexMakeCurrent()
{
    glfwMakeContextCurrent(nullptr);
    glfwMakeContextCurrent(window);
    
}

/* =====================================================================================
	main
===================================================================================== */
int main(void)
{

    /* Initialize the library */
    if (!glfwInit())
        return -1;


#if USING_OPENGLES
    // OpenGL ES 2.0
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#else
    //es2 is no profile! don't use core profile.
   // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 600, "Sprite Studio Viewer Sample", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback( window , framebuffer_size_callback );
	update_view( window );

	/* Create task system */
	new task_manager_singleton();
	SSTextureFactory*	texfactory = new SSTextureFactory( new SSTextureGL() );

    // Initialize AntTweakBar
    TwInit(TW_OPENGL, NULL);


    // Set GLFW event callbacks
    // - Redirect window size changes to the callback function WindowSizeCB
    glfwSetWindowSizeCallback( window , framebuffer_size_callback );
    // - Directly redirect GLFW mouse button events to AntTweakBar
    glfwSetMouseButtonCallback( window , MouseButtonCB );
    // - Directly redirect GLFW mouse position events to AntTweakBar
	glfwSetCursorPosCallback( window , MousePosCB);
    // - Directly redirect GLFW mouse wheel events to AntTweakBar
    
	
	//glfwSetMouseWheelCallback((GLFWmousewheelfun)TwEventMouseWheelGLFW);
	glfwSetScrollCallback( window , MouseScrollCB );
    // - Directly redirect GLFW key events to AntTweakBar
    glfwSetKeyCallback(window , KeyFunCB);
    // - Directly redirect GLFW char events to AntTweakBar

	glfwSetCharCallback(window ,CharFunCB);

#ifdef _WIN32
	//initialize of GLEW
	int glew_error = glewInit();
	if ( glew_error != GLEW_OK ) {
		const GLubyte* err_str = glewGetErrorString( glew_error );

		glfwTerminate();
		task_manager_destroy();
		return -1;
	}
#endif

    //XPFileOpenDlg dlg;
    //dlg.Show();

	int state = 0;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        update_view( window );

        /* Render here */
		if (state == 0 )
		{
			initialize();
			state = 1;
		}else{
			update();
		}

		TwDraw();
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }

	delete texfactory;
	task_manager_destroy();
    glfwTerminate();

    return 0;
}

