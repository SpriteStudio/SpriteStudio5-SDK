#ifndef __SSTEXTURECCS__
#define __SSTEXTURECCS__

#include "../helper/IsshTexture.h"

class	SSTextureCCS : public ISSTexture
{
	CComPtr< CD3DTexture >		m_tex;
	CComPtr< CDirect3DHelper >	m_pD3DHelper;

public:
	SSTextureCCS( CDirect3DHelper* pD3DHelper ):
		m_pD3DHelper( pD3DHelper ){}
	virtual ~SSTextureCCS(){}
	bool Load( const char* filename );	

	virtual int	getWidth() { return m_tex->GetWidth(); }
	virtual int	getHeight() { return  m_tex->GetHeight(); }
	
	virtual ISSTexture* create(){ return new SSTextureCCS( m_pD3DHelper ); }
	CD3DTexture* getTexture(){ return m_tex; }
};

#endif
