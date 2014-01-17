//
#include "bind_SsAnimeDecoder.h"


#include "ssplayer_animedecode.h"



Bind_SsAnimeDecoder::Bind_SsAnimeDecoder() : m_decoder(0)
{
	puts("construct Bind_SsAnimeState");	
	//m_decoder = new SsAnimeDecoder();

}

Bind_SsAnimeDecoder::~Bind_SsAnimeDecoder()
{
	puts( "destruct Bind_SsAnimeState" );
}


bool Bind_SsAnimeDecoder::debug()
{
	puts( "debug Bind_SsAnimeState" );

	if ( m_decoder )
	{
		PYDEBUG_PRINTF( "decoder:anime endframe = %d" , this->m_decoder->getAnimeEndFrame() );

		this->m_decoder->setPlayFrame(0);
		this->m_decoder->update();

		//データを出してみる

		std::list<SsPartState*>& llist = this->m_decoder->getPartSortList();

		PYDEBUG_PRINTF( "==position output test==");
		int p_no = 0;
		foreach( std::list<SsPartState*> , llist , e )
		{
			SsPartState* state = (*e);
			PYDEBUG_PRINTF( "[part%03d]position(%f,%f)" , p_no , state->position.x , state->position.y );
			p_no++;
		}
	}


	return true;
}
