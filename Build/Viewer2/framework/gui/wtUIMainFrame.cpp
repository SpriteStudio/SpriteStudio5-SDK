#include "../wtafx.h"
#include "wtUIMainFrame.h"
#include "wtui.h"

//GL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../../nanovg/nanovg.h"

#define NANOVG_GL2_IMPLEMENTATION
#include "../../nanovg/nanovg_gl.h"
#include "../../nanovg/nanovg_gl_utils.h"

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


static wtUIMainFrame* singletonMainFrame = 0;


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
	singletonMainFrame->resize( 0 , 0 , width , height );
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
	singletonMainFrame->mouseMove( x , y );
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



void renderPattern(NVGcontext* vg, NVGLUframebuffer* fb, float t, float pxRatio)
{
	int winWidth, winHeight;
	int fboWidth, fboHeight;
	int pw, ph, x, y;
	float s = 20.0f;
	float sr = (cosf(t)+1)*0.5f;
	float r = s * 0.6f * (0.2f + 0.8f * sr);

	if (fb == NULL) return;

	nvgImageSize(vg, fb->image, &fboWidth, &fboHeight);
	winWidth = (int)(fboWidth / pxRatio);
	winHeight = (int)(fboHeight / pxRatio);

	// Draw some stull to an FBO as a test
	nvgluBindFramebuffer(fb);
	glViewport(0, 0, fboWidth, fboHeight);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	nvgBeginFrame(vg, winWidth, winHeight, pxRatio);

	pw = (int)ceilf(winWidth / s);
	ph = (int)ceilf(winHeight / s);

	nvgBeginPath(vg);
	for (y = 0; y < ph; y++) {
		for (x = 0; x < pw; x++) {
			float cx = (x+0.5f) * s;
			float cy = (y+0.5f) * s;
			nvgCircle(vg, cx,cy, r);
		}
	}
	nvgFillColor(vg, nvgRGBA(220,160,0,200));
	nvgFill(vg);

	nvgEndFrame(vg);
	nvgluBindFramebuffer(NULL);
}
int	wtUIMainFrame::start()
{
	singletonMainFrame = this;
	this->setPause(false);
	this->setShow(true);

	NVGLUframebuffer* fb = NULL;
	GLFWwindow* window;
	DemoData data;
	vg = NULL;
	PerfGraph fps,cpuGraph;
	double prevt = 0;
	float pxRatio;


	if (!glfwInit()) {
		printf("Failed to init GLFW.");
		return -1;
	}

	initGraph(&fps, GRAPH_RENDER_FPS, "Frame Time");
	initGraph(&cpuGraph, GRAPH_RENDER_MS, "CPU Time");

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

	if (loadDemoData(vg, &data) == -1)
		return -1;

	int winWidth, winHeight;
	int fbWidth, fbHeight;
	glfwGetWindowSize(window, &winWidth, &winHeight);
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
	pxRatio = (float)fbWidth / (float)winWidth;
	fb = nvgluCreateFramebuffer(vg, (int)(100*pxRatio), (int)(100*pxRatio));
	if (fb == NULL) {
		printf("Could not create FBO.\n");
		//return -1;
	}

	//フォントの読み込み
	this->fontIcons = nvgCreateFont(vg, "icons", "./example/entypo.ttf");
	if (this->fontIcons == -1) {
		printf("Could not add font icons.\n");
		//return -1;
	}
	this->fontNormal = nvgCreateFont(vg, "sans", "./example/Roboto-Regular.ttf");
	if (this->fontNormal == -1) {
		printf("Could not add font italic.\n");
		//return -1;
	}
	this->fontBold = nvgCreateFont(vg, "sans-bold", "./example/Roboto-Bold.ttf");
	if (this->fontBold == -1) {
		printf("Could not add font bold.\n");
		//return -1;
	}
	this->fontJapanease = nvgCreateFont(vg, "japanease", "./example/TanukiMagic.ttf");
	if (this->fontBold == -1) {
		printf("Could not add font bold.\n");
		//return -1;
	}
	


	glfwSwapInterval(0);

	glfwSetTime(0);
	prevt = glfwGetTime();


	this->onCreate();

	//メインループを回す
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


		this->execute();
		this->execdraw();

		// Use the FBO as image pattern.
		if (fb != NULL) {
			NVGpaint img = nvgImagePattern(vg, 0, 0, 100, 100, 0, fb->image, NVG_REPEATX|NVG_REPEATY, 1.0f);
			nvgSave(vg);

			for (int i = 0; i < 20; i++) {
				nvgBeginPath(vg);
				nvgRect(vg, 10 + i*30,10, 10, winHeight-20);
				nvgFillColor(vg, nvgHSLA(i/19.0f, 0.5f, 0.5f, 255));
				nvgFill(vg);
			}

			nvgBeginPath(vg);
			nvgRoundedRect(vg, 140 + sinf(t*1.3f)*100, 140 + cosf(t*1.71244f)*100, 250, 250, 20);
			nvgFillPaint(vg, img);
			nvgFill(vg);
			nvgStrokeColor(vg, nvgRGBA(220,160,0,255));
			nvgStrokeWidth(vg, 3.0f);
			nvgStroke(vg);

			nvgRestore(vg);
		}

		//renderDemo(vg, mx,my, winWidth,winHeight, t, blowup, &data);
		renderGraph(vg, 310,0, &fps);
		renderGraph(vg, 5+200+315,0, &cpuGraph);


		nvgEndFrame(vg);
		//cpuTime = glfwGetTime() - t;
		//updateGraph(&cpuGraph, cpuTime);
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
	//if ( !o->isPause() && o->isShow() )
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

//---------------------------------------------
void	wtUIMainFrame::execute()
{
	execute_sub( this );
}

//---------------------------------------------
void	wtUIMainFrame::execdraw()
{
	draw_sub( this );
}

//---------------------------------------------
void	wtUIMainFrame::onCreate()
{
	wtUIPanel* tt = new wtUIPanel();
	tt->caption = "Tools";
	tt->setShow(true);
	tt->setPause(false);
	tt->vg = this->vg;
	tt->height = 64;
	tt->DockStatus = wtUI::Dock::top;
	this->appendchild( tt );

	wtUIPanel* w = new wtUIPanel();
	w->caption = "Explorer";
	w->setShow(true);
	w->setPause(false);
	w->vg = this->vg;
	w->DockStatus = wtUI::Dock::left;
	this->appendchild( w );

	wtUIPanel* w2 = new wtUIPanel();
	w2->caption = "Frame Control";
	w2->setShow(true);
	w2->setPause(false);
	w2->vg = this->vg;
	w2->DockStatus = wtUI::Dock::bottom;
	w2->height = 320;
	this->appendchild( w2 );

	wtUIPanel* w3 = new wtUIPanel();
	w3->caption = "Attributes";
	w3->setShow(true);
	w3->setPause(false);
	w3->vg = this->vg;
	w3->DockStatus = wtUI::Dock::right;
	w3->width = 350;
	this->appendchild( w3 );


	wtUIPanel* w4 = new wtUIPanel();
	w4->caption = "Layout";
	w4->setShow(true);
	w4->setPause(false);
	w4->vg = this->vg;
	w4->DockStatus = wtUI::Dock::left;
	w4->AlignStatus = wtUI::Align::full;

	this->appendchild( w4 );



}

//---------------------------------------------
//	リサイズの通知
//---------------------------------------------
void	wtUIMainFrame::resize( int x , int y , int w , int h )
{
	this->onResize( w , h );

	//Dock、アライメント、アンカー再計算
	std::vector<wtUIObject*> updatelist;
	
	//自分の子のみを取得
	wtUIObject* o = this->get_child();
	while(o)
	{
		updatelist.push_back(o);
		o = o->get_sibling();
	}
	
	//dockのプライオリティ順にソートする
	
	//現在は並んでる順に

	int sx = 0;
	int sy = 0;
	int ex = w;
	int ey = h;

	//位置、サイズの計算を行う
	for ( int i = 0 ; i < updatelist.size() ; i++ )
	{
		//位置の調整
		wtUIObject* obj = updatelist[i];
		wtUIPanel* panel = static_cast<wtUIPanel*>(obj);

		if ( panel->AlignStatus == wtUI::Align::full )
		{
			panel->width = w;
			panel->height = h;
		}

		if ( panel->DockStatus == wtUI::Dock::left )
		{
			int ww = panel->width;
			if ( ww > ( ex - sx ) )
			{
				//残り範囲より大きい時
				ww = ex-sx;
			}
			updatelist[i]->resize( sx , sy , ww , ey-sy );
			sx+=panel->width;
		}
		if ( panel->DockStatus == wtUI::Dock::right )
		{
			updatelist[i]->resize( ex - panel->width , sy , panel->width , ey-sy );
			ex-=panel->width;
		}

		if ( panel->DockStatus == wtUI::Dock::bottom )
		{
			updatelist[i]->resize( sx , ey - panel->height , ex - sx , panel->height );
			ey-=panel->height;
		}
		if ( panel->DockStatus == wtUI::Dock::top )
		{
			updatelist[i]->resize( sx , sy, ex - sx , panel->height );
			sy+=panel->height;
		}
		//updatelist[i]->onResize( w , h );
	}
}


//---------------------------------------------
//	リサイズの通知
//---------------------------------------------
void	wtUIMainFrame::onResize(int w, int h)
{



}


//--------------------------------------------------------
// test
//--------------------------------------------------------
int main(void)
{

	//--------------------------------------------
	//メインフレームの作成
	wtUIMainFrame* mainframe = new wtUIMainFrame();

	mainframe->start();

	delete mainframe;

}