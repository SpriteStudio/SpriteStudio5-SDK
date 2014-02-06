//
#include "bind_Animation.h"

Bind_SsCellValue*	Bind_SsPartState::cellValue()
{
	Bind_SsCellValue* temp = new Bind_SsCellValue();

	temp->bind( &this->bind_inst->cellValue );

	return temp;
}


