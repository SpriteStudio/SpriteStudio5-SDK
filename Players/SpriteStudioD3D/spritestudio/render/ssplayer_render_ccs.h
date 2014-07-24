#ifndef __SSPLAYER_RENDER_CCS__
#define __SSPLAYER_RENDER_CCS__

#include "../Animator/ssplayer_render.h"

struct SsPartState;

class SsRenderCCS : public ISsRenderer
{
public:
	enum{
		MATRIX_SIZE  = 16,
		INDEX_OFFSET = 100000,
	};

private:
	//static bool	m_isInit;
	CComPtr< CDirect3DHelper >			m_pD3DHelper;
	CComPtr< CD3DRender >				m_drawer;
	float								m_localmatrix[ MATRIX_SIZE ];
	int									m_drawcount;
	int									m_index;
	SsBlendType::_enum					m_blendtype;

public:
	SsRenderCCS( CDirect3DHelper* pD3DHelper ):
		m_pD3DHelper( pD3DHelper ){}
	virtual ~SsRenderCCS(){}

	virtual void	initialize();
	virtual void	renderSetup();
	virtual void	renderPart( SsPartState* state );
	CD3DRender*		getDrawer(){ return m_drawer; }
	void			setIndex( int n ){ m_index = n; }
	void			clearDrawCount( void ){ m_drawcount = 0; }
	static void		changeAnimation( int packIndex, int animeIndex, SsAnimeDecoder* player, SsCellMapList* cellmap, SsProject* proj );

	void setMatrix( float* matrix )
	{
		for( int i = 0; i < MATRIX_SIZE; i++ ){
			m_localmatrix[ i ] = matrix[ i ];
		}
	}
};

#endif
