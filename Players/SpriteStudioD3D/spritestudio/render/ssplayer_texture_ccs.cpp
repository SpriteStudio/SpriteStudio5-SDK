#include "stdafx.h"
#include <D3D9.h>
#include <D3DX9.h>
#include "../Helper/ICCSInterface.h"
#include "../Helper/Direct3D.h"
#include "../Helper/CGlobalObject.h"
#include "../Helper/CGWindowBase.h"
#include "../Helper/CD3DTexture.h"
#include "../Helper/CD3DRender.h"
#include "ssplayer_texture_ccs.h"

bool SSTextureCCS::Load( const char* filename )
{
	CComPtr< CD3DTexture > tex;
	tex.Attach( new CD3DTexture( m_pD3DHelper ) );
	if( !tex->CreateTextureFromFile( filename ) ){
		m_tex = NULL;
		return false;
	}
	m_tex = tex;
	return true;
}
