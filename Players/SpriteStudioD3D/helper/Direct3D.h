/*
 * @概要		DirectX管理クラス
 *
 * $Id: $
 *
 * @author		諒
 * @since		2014/07/18
 */

#pragma once

#include <unknwn.h>
#include "ICCSInterface.h"

// π
const float PI = 3.141592653589793238462643383279f;

// デバイスモードの定義
enum D3DDEVICEMODE{
	D3DDEVICE_TNL,
	D3DDEVICE_HAL,
	D3DDEVICE_HEL,

	D3DDEVICE_SIZE
};

// ======================================================================
//  列挙型定義
// ======================================================================
enum D3DOBJTYPE{
	D3DOBJ_RENDER,
	D3DOBJ_EFFECT,
	D3DOBJ_TEXTURE,
	D3DOBJ_BACKBUFFER,

	D3DOBJ_SIZE
};

// レンダリングタイプの定義
enum D3DRENDERTYPE{
	D3DRENDER_MUL,
	D3DRENDER_ADD,
	D3DRENDER_DIF,

	D3DRENDER_SIZE
};

// ブレンドタイプの定義
enum D3DBLENDTYPE{
	D3DBLEND_NML,
	D3DBLEND_ADD,
	D3DBLEND_SUB,
	D3DBLEND_MUL,
	D3DBLEND_OVR,
	D3DBLEND_CHI,
	D3DBLEND_CHD,
	D3DBLEND_CHM,
	D3DBLEND_ACI,
	D3DBLEND_ACD,
	D3DBLEND_ACM,

	D3DBLEND_SIZE
};

// 外部変更不可パラメータ
struct D3DCONSTPARAMETER{
	UINT	nMaxTextureWidth;
	UINT	nMaxTextureHeight;
	UINT	nMaxPrimitiveCount;
	UINT	nMaxVertexIndex;
	bool	bTextureSizePow2;
};

// 4パラメータ
struct D3DFOURPARAM{
	float		fSrcX[ 4 ];
	float		fSrcY[ 4 ];
	float		fDestX[ 4 ];
	float		fDestY[ 4 ];
	float		fDestZ[ 4 ];
	float		fRHW[ 4 ];
	D3DCOLOR	Color[ 4 ];
};

// 頂点FVF
const DWORD VERTEXFVF = ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );

class CDirect3DHelper:public CCS::ICCSInterface
{
public:
	CDirect3DHelper( void ):
		m_nRefCount( 1 )
	{
		InitDeviceState();
	}
	virtual ~CDirect3DHelper( void ){}

public:
	static void InitDirect3DObject( void );
	static void TermDirect3DObject( void );

	bool InitDirect3DDevice( HWND hWnd, D3DDEVICEMODE DevMode, UINT nWidth, UINT nHeight, bool bVSync );
	void TermDirect3DDevice( void );
	IDirect3DDevice9* GetDevice( void ){ return m_pD3DDevice; }
	D3DCONSTPARAMETER* GetConstParameter( void ){ return &m_ConstParameter; }
	void InitDeviceState( void );

	void STDCALLMETHOD ClearScreen( DWORD dwBackColor );
	bool STDCALLMETHOD Flip( void );
	bool STDCALLMETHOD BeginDraw( void );
	void STDCALLMETHOD EndDraw( void );

	static void	MatrixRotation( D3DMATRIX* pDMatrix, float fAngleX, float fAngleY, float fAngleZ );
	static void	TransformNormal( D3DVECTOR* pDVector, const D3DVECTOR* pSVector, const D3DMATRIX* pSMatrix );

	void  SetBlendOp( DWORD d );
	DWORD  GetBlendOp( void ){ return m_nBlendOp; }
	void  SetSrcBlend( DWORD d );
	DWORD GetSrcBlend( void ){ return m_nSrcBlend; }
	void  SetDstBlend( DWORD d );
	DWORD GetDstBlend( void ){ return m_nDstBlend; }
	void  SetBlendOpAlpha( DWORD d );
	DWORD  GetBlendOpAlpha( void ){ return m_nBlendOpAlpha; }
	void  SetSrcBlendAlpha( DWORD d );
	DWORD GetSrcBlendAlpha( void ){ return m_nSrcBlendAlpha; }
	void  SetDstBlendAlpha( DWORD d );
	DWORD GetDstBlendAlpha( void ){ return m_nDstBlendAlpha; }

	void  SetAlphaBlendEnable( DWORD d );
	DWORD GetAlphaBlendEnable( void ){ return m_nAlphaBlendEnable; }
	void  SetSeparateAlphaBlendEnable( DWORD d );
	DWORD GetSeparateAlphaBlendEnable( void ){ return m_nSeparateAlphaBlendEnable; }
	void  SetCullMode( DWORD d );
	DWORD GetCullMode( void ){ return m_nCullMode; }
	void  SetLighting( DWORD d );
	DWORD GetLighting( void ){ return m_nLighting; }
	void  SetAlphaTestEnable( DWORD d );
	DWORD GetAlphaTestEnable( void ){ return m_nAlphaTestEnable; }
	void  SetAlphaRef( DWORD d );
	DWORD GetAlphaRef( void ){ return m_nAlphaRef; }
	void  SetAlphaFunc( DWORD d );
	DWORD GetAlphaFunc( void ){ return m_nAlphaFunc; }
	void  SetTexColorOp( DWORD d );
	DWORD GetTexColorOp( void ){ return m_nTexColorOp; }
	void  SetTexColorArg1( DWORD d );
	DWORD GetTexColorArg1( void ){ return m_nTexColorArg1; }
	void  SetTexColorArg2( DWORD d );
	DWORD GetTexColorArg2( void ){ return m_nTexColorArg2; }
	void  SetTexAlphaOp( DWORD d );
	DWORD GetTexAlphaOp( void ){ return m_nTexAlphaOp; }
	void  SetTexAlphaArg1( DWORD d );
	DWORD GetTexAlphaArg1( void ){ return m_nTexColorArg1; }
	void  SetTexAlphaArg2( DWORD d );
	DWORD GetTexAlphaArg2( void ){ return m_nTexColorArg2; }
	void  SetScaleState( DWORD d );
	DWORD GetScaleState( void ){ return m_nScaleState; }

public:
	virtual ULONG STDCALLMETHOD AddRef( void )
	{
		return ++m_nRefCount;
	}
	virtual ULONG STDCALLMETHOD Release( void )
	{
		if( --m_nRefCount == 0 ){
			delete this;
			return 0;
		}
		return m_nRefCount;
	}
	virtual ULONG STDCALLMETHOD GetRefCount( void )
	{
		return m_nRefCount;
	}

private:
	int m_nRefCount;
	CComPtr< IDirect3DDevice9 > m_pD3DDevice;
	D3DCONSTPARAMETER m_ConstParameter;
	D3DPRESENT_PARAMETERS m_D3DParam;
	static CComPtr< IDirect3D9 > g_pDirect3D;

	//RenderState
	DWORD m_nCullMode;
	DWORD m_nLighting;
	DWORD m_nTexColorOp;
	DWORD m_nTexColorArg1;
	DWORD m_nTexColorArg2;
	DWORD m_nTexAlphaOp;
	DWORD m_nTexAlphaArg1;
	DWORD m_nTexAlphaArg2;
	DWORD m_nBlendOp;
	DWORD m_nSrcBlend;
	DWORD m_nDstBlend;
	DWORD m_nBlendOpAlpha;
	DWORD m_nSrcBlendAlpha;
	DWORD m_nDstBlendAlpha;
	DWORD m_nAlphaBlendEnable;
	DWORD m_nSeparateAlphaBlendEnable;
	DWORD m_nAlphaTestEnable;
	DWORD m_nAlphaRef;
	DWORD m_nAlphaFunc;
	DWORD m_nScaleState;
};
