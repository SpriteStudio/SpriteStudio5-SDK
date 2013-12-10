#ifndef __RS_DX9__
#define __RS_DX9__

//#include "Draw.h"
//#include "DirectDraw_Font.h"



#include <d3d9.h>
#include <vector>
#include <string>



#pragma	comment(lib, "d3d9.lib")
//#pragma	comment(lib, "d3dx9.lib")


#define		RELEASE(o)		if (o){o->Release();o=NULL;}
#define		PI				(3.1415926f)		//!< π
#define		DEGtoRAD(deg)		((float)deg * 0.017453292f)	//!< デグリーからラジアンへ
#define		RADtoDEG(rad)		((float)rad * 57.29578049f)	//!< ラジアンからデグリーへ
#define		SAFE_DELETE(p)			{ if(p){ delete (p); (p)=NULL; } }		//!< メモリを解放
#define		SAFE_DELETE_ARRAY(p)	{ if(p){ delete [](p); (p)=NULL; } }	//!< 配列でとったメモリを解放
#define		SAFE_RELEASE(p)			{ if(p){ (p)->Release(); (p)=NULL; } }	//!< DXオブジェクトを解放


//---------------------------------------------------------------------------------------------------------
//
//
//---------------------------------------------------------------------------------------------------------
typedef void (*CDirectDrawCALLBACK)();				//ハッシュ値作成 	

class	IDirectDrawResourceManager
{
	virtual BOOL	Dispose() = 0;
	virtual BOOL	Restore() = 0;
	virtual BOOL	Release() = 0;
};

#if 0
class	CDirectDraw_RenderState
{
private:
	DWORD	m_valuePrevious[300];
	DWORD	m_valueNew[300];

public:
	void	SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
	{
		m_valueNew[(int)State] = Value;
	}
	void	Update()
	{
		HRESULT	hr;
//			LPDIRECT3DDEVICE9	Device = GetD3DDevice();
		DWORD	valueNew;
		for(int i=0; i<300; i++)
		{
			valueNew = m_valueNew[i];
			if((DWORD)0x7fffffff != valueNow)
			{
				if(m_valuePrevious[i] != valueNew)
				{	/* 値が同じ時には書き換えない */
//						hr = Device->SetRenderState((D3DRENDERSTATETYPE)i, valueNew);
				}
			}
			m_valuePrevious[i] = valueNew;
		}
	}
	void	Clear()
	{
		for(int i=0; i<300; i++)
		{
			m_valuePrevious[i] = (DWORD)0x7fffffff;
			m_valueNew[i] = (DWORD)0x7fffffff;
		}
	}

	CDirectDraw_RenderState()
	{
		Clear();
	}
	~CDirectDraw_RenderState()
	{
	}
};
#endif

class CDirectDrawSimple 
{
public:
	enum DISPLAYMODE
	{
		DISPLAYMODE_NONE,		//!< よくわからない
		DISPLAYMODE_WINDOW,		//!< ウインドウモード
		DISPLAYMODE_FULL,		//!< フルスクリーンモード
	};

private:	
	static CDirectDrawSimple*	myInst;
	LPDIRECT3D9	m_pD3D;

	int	m_dwWidth;
	int	m_dwHeight;

	DWORD	m_VertexShaderVersion;
	DWORD	m_PixelShaderVersion;

	D3DPRESENT_PARAMETERS	m_d3dParam_current;
	D3DPRESENT_PARAMETERS	m_d3dParam_window;			
	D3DPRESENT_PARAMETERS	m_d3dParam_full;

	LPDIRECT3DDEVICE9		m_pd3dDev;
	LPDIRECT3DSURFACE9		m_pBackBuffer;		
	LPDIRECT3DSURFACE9		m_currentRenderTarget;

	HWND					m_hwnd;
	int			m_Vertextype;
	static bool	m_DeviceLost;

	bool		m_pause;
	DISPLAYMODE	m_current_displaymode;

	RECT  g_rectWindow;

	BYTE	bgcolorR;
	BYTE	bgcolorG;
	BYTE	bgcolorB;

		
	CDirectDrawCALLBACK		m_ReleaseDeviceCallback;
	CDirectDrawCALLBACK		m_RestoreDeviceCallback;

private:
	void	CreateCaps();
	BOOL	Create( void* HWND , int ScreenWidth , int ScreenHeight );

public:
	CDirectDrawSimple()
	{
		m_pause = false;
	}

	CDirectDrawSimple(
				HWND HWND , 
				int ScreenWidth , 
				int ScreenHeight, 
				CDirectDrawSimple::DISPLAYMODE displaymode = DISPLAYMODE_WINDOW )
	{
		m_pBackBuffer = 0;
		m_DeviceLost = false;
		m_current_displaymode = displaymode;
		Create( HWND , ScreenWidth , ScreenHeight );
		m_hwnd = HWND;
		m_pause = false;
		myInst = this;

	}

	~CDirectDrawSimple()
	{
		Cleanup();
		myInst = 0;
	}

	BOOL	Cleanup();

	virtual HRESULT	BeginScene();
	virtual	HRESULT	EndScene();

	HWND			GetHWnd(){ return m_hwnd; }
	LPDIRECT3D9		GetD3D(){ return m_pD3D; }
	LPDIRECT3DDEVICE9 GetDevice(){ return m_pd3dDev;} 

	static	CDirectDrawSimple*	GetInstance(){return myInst;}


	int	GetScreenWidth(){ return m_dwWidth;}
	int	GetScreenHeight(){ return m_dwHeight;}
	void	SetScreenWidth( int w ) { m_dwWidth = w;}
	void	SetScreenHeight( int h ) { m_dwHeight = h;}



	LPDIRECT3DSURFACE9 getBackBuffer();

	void	SetRenderTarget(int index , LPDIRECT3DSURFACE9 surface , LPDIRECT3DSURFACE9 depath = 0)
	{
		m_currentRenderTarget = surface;
		m_pd3dDev->SetRenderTarget( index , surface );
		if ( depath )
		{
			m_pd3dDev->SetDepthStencilSurface( depath );
		}
	}

	LPDIRECT3DSURFACE9 GetRenderTarget()
	{
		return m_currentRenderTarget;
	}

	void	SetPause( bool flag )
	{
		m_pause = flag;
	}

	virtual void	ChangeDisplayMode( DISPLAYMODE mode );
	DISPLAYMODE		GetCurrentDisplayMode(){ return m_current_displaymode; }
	virtual BOOL	DeviceChk();

	void	SetBackGroundColor( BYTE r , BYTE g , BYTE b )
	{
		bgcolorR = r;
		bgcolorG = g;
		bgcolorB = b;

	}

	DWORD GetVSVersion(){return m_VertexShaderVersion;	}
	DWORD GetPSVersion(){return m_PixelShaderVersion;	}
	void	SetReleaseDeviceCallBackFunc(CDirectDrawCALLBACK func ) { m_ReleaseDeviceCallback = func; }
	void	SetRestoreDeviceCallBackFunc(CDirectDrawCALLBACK func ) { m_RestoreDeviceCallback = func; }

//		CDirectDraw_RenderState*	GetRenderState();
 
	void	DeviceReset();
};

/*
void	Setup2DMatrix( int width , int height , float center_posx , float center_posy );
void	Setup2DMatrix( int width , int height );
*/


#endif
