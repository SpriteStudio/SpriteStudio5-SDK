#include "ssarchiver.h"
#include "ssstring_uty.h"

bool	SsXmlIArchiver::dc_attr( const char* name , SsString& member )
{
	AR_SELF_CHECK();
	const char* v = 0;

	v = getxml()->Attribute( name  );
	if ( v != 0 ) member= v;


	return true;
}
bool	SsXmlIArchiver::dc_attr( const char* name , int& member )
{
	AR_SELF_CHECK();

	const char* v = getxml()->Attribute( name  );

	member = atoi(v);

	return true;
}

bool	SsXmlIArchiver::dc( const char* name , int& member )
{

	AR_SELF_CHECK();
	SsString str;
	dc( name , str );
	member = atoi( str.c_str() );

	return true;
}

bool	SsXmlIArchiver::dc( const char* name , float& member )
{
	AR_SELF_CHECK();
	SsString str;
	dc( name , str );
	member = (float)atof( str.c_str() );

	return true;
}

bool	SsXmlIArchiver::dc( const char* name , SsString& member )
{
	AR_SELF_CHECK();
	XMLElement* e = getxml()->FirstChildElement( name );
	if ( e )
	{
		if ( e->GetText() )
		{
			//Winではsjisへ変換する
			member = babel::utf8_to_sjis( e->GetText() );
		}else{
			member ="";
		}
		return true;
	}
	return false;
}

bool	SsXmlIArchiver::dc( const char* name , bool& member )
{
	AR_SELF_CHECK();
	member = false;
	XMLElement* e = getxml()->FirstChildElement( name );
	if ( e )
	{
		int ret = GetTextToInt( e , 0 );
		if ( ret == 1)member = true;
		return true;
	}

	return false;
}

bool	SsXmlIArchiver::dc( const char* name , std::vector<SsString>& list )
{
	AR_SELF_CHECK();
	list.clear();
	XMLElement* e = getxml()->FirstChildElement( name );
	if (e==0)return false;
	e = e->FirstChildElement( "value" );
	while( e )
	{
		const char* txt = e->GetText();
		//Winではsjisへ変換する
		std::string sjis_str = babel::utf8_to_sjis( txt );

		list.push_back( sjis_str );
		e = e->NextSiblingElement();
	}

	return true;
}

bool	SsXmlIArchiver::dc( const char* name , SsPoint2& member )
{
	AR_SELF_CHECK();

	XMLElement* e = getxml()->FirstChildElement( name );

	if ( e )
	{
		std::string str = e->GetText();
		return StringToPoint2( str , member );

	}
	return false;

}

bool	SsXmlIArchiver::dc( const char* name , SsCurve& member )
{
	AR_SELF_CHECK();

	XMLElement* e = getxml()->FirstChildElement( name );

	if ( e )
	{
		std::vector<SsString>	str_list;
		split_string( e->GetText() , ' ' , str_list );
		if ( str_list.size() < 4 )
		{
			return false;
		}else{
			member.startTime = (float)atof( str_list[0].c_str() );
			member.startValue = (float)atof( str_list[1].c_str() );
			member.endTime = (float)atof( str_list[2].c_str() );
			member.endValue = (float)atof( str_list[3].c_str() );		

			return true;
		}
	}
	return false;
}


bool	StringToPoint2( const std::string& str , SsPoint2& point )
{
	std::vector<SsString>	str_list;
	split_string( str , ' ' , str_list );
	if ( str_list.size() < 2 )
	{
		point.x = 0;
		point.y = 0;
		return false;
	}else{
		point.x = (float)atof( str_list[0].c_str() );
		point.y = (float)atof( str_list[1].c_str() );
	}

	return true;
}


bool	StringToIRect( const std::string& str , SsIRect& rect )
{
	std::vector<SsString>	str_list;
	split_string( str , ' ' , str_list );
	if ( str_list.size() < 4 )
	{
		rect.x = 0;
		rect.y = 0;
		rect.w = 0;
		rect.h = 0;
		return false;
	}else{
		rect.x = (int)atof( str_list[0].c_str() );
		rect.y = (int)atof( str_list[1].c_str() );
		rect.w = (int)atof( str_list[2].c_str() );
		rect.h = (int)atof( str_list[3].c_str() );
	}

	return true;
}


void	SsArchiverInit()
{
	babel::init_babel();
	
}
