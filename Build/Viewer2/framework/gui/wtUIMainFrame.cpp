#include "../wtafx.h"
#include "wtUIMainFrame.h"
#include "wtui.h"

//GL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../../nanovg/nanovg.h"

#define NANOVG_GL2_IMPLEMENTATION
#include "../../nanovg/nanovg_gl.h"

#include "../../nanovg/demo.h"
#include "../../nanovg/perf.h"



#ifdef _WIN32
#pragma comment(lib, "glew32.lib")

#ifdef _DEBUG
#pragma comment(lib, "glfw3d.lib")
#else
#pragma comment(lib, "glfw3.lib")
#endif
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
//#pragma comment(lib, "AntTweakBar.lib")
#else
//#define NULL (0)
#endif


//コールバック
void errorcb(int error, const char* desc)
{
	printf("GLFW error %d: %s\n", error, desc);
}

int blowup = 0;
int screenshot = 0;
int premult = 0;

static void key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	NVG_NOTUSED(scancode);
	NVG_NOTUSED(mods);
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		blowup = !blowup;
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		screenshot = 1;
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		premult = !premult;
}


void	framebuffer_size_callback( GLFWwindow* window , int width , int height )
{
//	update_view( window );
}

//typedef void (* GLFWmousebuttonfun)(GLFWwindow*,int,int,int);
void MouseButtonCB( GLFWwindow*,int button ,int action ,int mods)
{
	//TwEventMouseButtonGLFW( button , action );
}

//typedef void (* GLFWcursorposfun)(GLFWwindow*,double,double);
void	MousePosCB(GLFWwindow*,double x ,double y)
{
	//TwEventMousePosGLFW( (int)(x*displayscale) , (int)(y*displayscale) );
}

//typedef void (* GLFWkeyfun)(GLFWwindow*,int,int,int,int);
void	KeyFunCB( GLFWwindow* window,int key,int scancode,int action,int mods)
{
//	TwEventKeyGLFW( key , action );
//	TwEventCharGLFW( key  , action );

}

//typedef void (* GLFWcharfun)(GLFWwindow*,unsigned int);
void	CharFunCB( GLFWwindow* window,unsigned int character)
{
//	TwEventCharGLFW( character );

}

//typedef void (* GLFWscrollfun)(GLFWwindow*,double,double);

void	MouseScrollCB(  GLFWwindow* window, double x , double y )
{
//	TwEventMouseWheelGLFW( (int)y );
}

int	wtUIMainFrame::start()
{

	GLFWwindow* window;
	DemoData data;
	NVGcontext* vg = NULL;
	PerfGraph fps;
	double prevt = 0;

	if (!glfwInit()) {
		printf("Failed to init GLFW.");
		return -1;
	}

	initGraph(&fps, GRAPH_RENDER_FPS, "Frame Time");

	glfwSetErrorCallback(errorcb);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#ifdef DEMO_MSAA
	glfwWindowHint(GLFW_SAMPLES, 4);
#endif

	window = glfwCreateWindow(1000, 600, "Sprite Studio Viewer Sample2", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

//	glfwSetKeyCallback(window, key);
	
	//コールバックイベントのセット
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



	glfwMakeContextCurrent(window);
#ifdef NANOVG_GLEW
    if(glewInit() != GLEW_OK) {
		printf("Could not init glew.\n");
		return -1;
	}
#endif

#ifdef DEMO_MSAA
	vg = nvgCreateGL2(NVG_STENCIL_STROKES);
#else
	vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
#endif
	if (vg == NULL) {
		printf("Could not init nanovg.\n");
		return -1;
	}
/*
	if (loadDemoData(vg, &data) == -1)
		return -1;
*/

	glfwSwapInterval(0);

	glfwSetTime(0);
	prevt = glfwGetTime();


	while (!glfwWindowShouldClose(window))
	{
		double mx, my, t, dt;
		int winWidth, winHeight;
		int fbWidth, fbHeight;
		float pxRatio;

		t = glfwGetTime();
		dt = t - prevt;
		prevt = t;
		updateGraph(&fps, dt);

		glfwGetCursorPos(window, &mx, &my);
		glfwGetWindowSize(window, &winWidth, &winHeight);
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

		// Calculate pixel ration for hi-dpi devices.
		pxRatio = (float)fbWidth / (float)winWidth;

		// Update and render
		glViewport(0, 0, fbWidth, fbHeight);
		if (premult)
			glClearColor(0,0,0,0);
		else
			glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

		nvgBeginFrame(vg, winWidth, winHeight, pxRatio);

		//renderDemo(vg, mx,my, winWidth,winHeight, t, blowup, &data);
		//renderGraph(vg, 5,5, &fps);

		this->execute();
		this->draw();

		nvgEndFrame(vg);
/*
		if (screenshot) {
			screenshot = 0;
			saveScreenShot(fbWidth, fbHeight, premult, "dump.png");
		}
*/		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//freeDemoData(vg, &data);

	nvgDeleteGL2(vg);

	glfwTerminate();
	return 0;

}

static void	execute_sub(wtUIObject* o)
{
	if ( !o->isPause() )
	{
		o->before_update();
		o->update();
		o->after_update();

		if ( o->get_child() )
		{
			execute_sub( o->get_child()	);
		}
	}

	if ( o->get_sibling() )
	{
		execute_sub( o->get_sibling() );
	}

}
static void	draw_sub(wtUIObject* o)
{
	if ( !o->isPause() && o->isShow() )
	{
		o->draw();

		if ( o->get_child() )
		{
			draw_sub( o->get_child()	);
		}
	}

	if ( o->get_sibling() )
	{
		draw_sub( o->get_sibling() );
	}

}

void	wtUIMainFrame::execute()
{
	execute_sub( this );
}

void	wtUIMainFrame::draw()
{
	draw_sub( this );
}


//--------------------------------------------------------
// test
//--------------------------------------------------------
int main(void)
{

	//--------------------------------------------
	//メインフレームの作成
	wtUIMainFrame* mainframe = new wtUIMainFrame();

	mainframe->appendchild( new wtUIPanel() );

	mainframe->start();

}