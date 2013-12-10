#ifdef _WIN32
#include <d3d9.h>
#include <math.h>
#include <Windows.h>
#endif
#include <AntTweakBar.h>

#include "./helper/d3dsimple.h"
#include "./helper/tkTask.h"
#include "./helper/XPFileOpenDlg.h"
#include "sampleScene.h"


IDirect3D9 *            g_D3D = NULL;
IDirect3DDevice9 *      g_D3DDev = NULL;
D3DPRESENT_PARAMETERS   g_D3Dpp;


#ifdef _WIN32
#pragma comment(lib, "AntTweakBar.lib")
#else
#endif


#define USING_OPENGLES (0)

GLFWvidmode mode;   // GLFW video mode
TwBar *g_twbar;         // Pointer to a tweak bar
    
float bgColor[] = { 0.1f, 0.2f, 0.4f };         // Background color 

float displayscale = 1.0f;


LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );
HINSTANCE               g_hInst = NULL;
HWND                    g_hWnd = NULL;
//static CDirectDrawSimple*		s_CD3D;

//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )0 );
    wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "viewer_sample d3d9";
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )0 );
    if( !RegisterClassEx( &wcex ) )
        return E_FAIL;

    // Create window
    g_hInst = hInstance;
    RECT rc = { 0, 0, 640, 480 };
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
    g_hWnd = CreateWindow( "viewer_sample d3d9", "viewer_sample d3d9",
                           WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
                           NULL );
    if( !g_hWnd )
        return E_FAIL;

    ShowWindow( g_hWnd, nCmdShow );

    return S_OK;
}


LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;
    if( TwEventWin(hWnd, message, wParam, lParam) )
        return 0;   // Event has been handled by AntTweakBar

    switch( message )
    {
        case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

		case WM_SIZE:
			{
				int	w = LOWORD(lParam);
				int	h = HIWORD(lParam);

				CDirectDrawSimple* cd3d = CDirectDrawSimple::GetInstance();

				//CDirectDrawSimple*	GetInstance(){return myInst;}
				cd3d->SetScreenWidth( w );
				cd3d->SetScreenHeight( h );

				cd3d->DeviceReset();

			}
			break;
        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}


/* =====================================================================================
	update
===================================================================================== */
static void	update()
{
	//double time = glfwGetTime();

	task_manager_singleton::getinst()->exec_resist_tasks();

//	glClear( GL_COLOR_BUFFER_BIT );
//	glClearColor( bgColor[0] , bgColor[1] , bgColor[2] , 1.0f );

	task_manager_singleton::getinst()->draw_resist_tasks();

}


#if 1
extern HINSTANCE g_hInstance;
/* =====================================================================================
	main
===================================================================================== */
int main(void)
{

	//ウインドウの初期化を行います。
	InitWindow( g_hInstance , true );

	//Direct3Dの初期化を行います。
	CDirectDrawSimple CD3D(g_hWnd,640,480);
	CD3D.SetBackGroundColor( 0 , 0 , 255 );


	/* Create task system */
	new task_manager_singleton();


    // Initialize AntTweakBar
	if ( !TwInit(TW_DIRECT3D9, CD3D.GetDevice()) )
	{
		return 0;
	}


    TwBar *bar = TwNewBar("TweakBar");

    // Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
			CD3D.BeginScene();

			update();


			TwDraw();
			CD3D.EndScene();
        }
    }




	return 0;
}
#endif
