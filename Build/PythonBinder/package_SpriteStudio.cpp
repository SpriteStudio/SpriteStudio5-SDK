#include "package_SpriteStudio.h"





void PYDEBUG_PRINTF( const char* strFormat, ...   )
{
	char buffer[256];

	va_list arglist;
	va_start( arglist, strFormat);
	_vsnprintf( buffer, 1024, strFormat, arglist);
	va_end( arglist);

	puts( buffer );

}