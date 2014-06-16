#include "bind_Cellmap.h"

Bind_Cell*	Bind_Cellmap::getCell(int index)
{
	Bind_Cell* c  = new Bind_Cell();
	c->bind( bind_inst->cells[index] );

	return c;
}


