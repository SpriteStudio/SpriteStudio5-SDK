//
#include "package_SpriteStudio.h"
#include "bind_AnimePack.h"
#include "ssplayer_animedecode.h"
#include "bind_Animation.h"


Bind_SsCellValue*	Bind_SsPartState::cellValue()
{
	Bind_SsCellValue* temp = new Bind_SsCellValue();

	temp->bind( &this->bind_inst->cellValue );

	return temp;
}


				
Bind_SsAnimeDecoder*	Bind_SsPartState::getInstanceDecoder()
{
	Bind_SsAnimeDecoder* ad = new Bind_SsAnimeDecoder();
	ad->m_decoder = this->bind_inst->refAnime;

	return ad;
	
}