#include "stdafx.h"

#pragma warning( push )
#pragma warning( disable: 4100 )//引数は関数の本体部で 1 度も参照されません。

#include "ssloader.h"
#include "sstypes.h"
#include "ssattribute.h"

const SsKeyframe*	SsAttribute::firstKey()
{
	if ( key.empty() )
		return 0;
		
	return key_dic.begin()->second;

}

///時間から左側のキーを取得
const SsKeyframe*	SsAttribute::findLeftKey( int time )
{

	static SsKeyframe ret;
	if ( key.empty() )
		return 0;
    
	AttributeKeyDic::const_iterator it = key_dic.lower_bound(time);
        
    if ( it == key_dic.end())
    {
		it = --it;
        SsKeyframe* key = it->second;
        return key;
    }
    
		
	if ( it->first == time ) return it->second;
	if ( key_dic.begin() == it )
	{
		if ( it->first > time ) return 0;
	}else{
		it = --it;
	}

	if ( it->first > time ) return 0;

	return it->second;
}

//時間から右側のキーを取得する
const SsKeyframe*	SsAttribute::findRightKey( int time )
{

	static SsKeyframe ret;
	if ( key.empty() )
		return 0;

	AttributeKeyDic::const_iterator it = key_dic.upper_bound(time);
	if (it == key_dic.end())
		return 0;
	return it->second;

}


//頂点カラーアニメデータの取得
void	GetSsColorValue( const SsKeyframe* key , SsColorAnime& v )
{
	SsColorBlendTarget::_enum target;
	__StringToEnum_( key->value["target"].get<SsString>() , target );
		SsBlendType::_enum blendtype;
	__StringToEnum_( key->value["blendType"].get<SsString>() , blendtype);

	v.blendType = blendtype;
	v.target = target;

	if ( target == SsColorBlendTarget::vertex )
	{
		SsHash lt = key->value["LT"].get<SsHash>();
		SsHash rt = key->value["RT"].get<SsHash>();
		SsHash lb = key->value["LB"].get<SsHash>();
		SsHash rb = key->value["RB"].get<SsHash>();

		ConvertStringToSsColor( lt["rgba"].get<SsString>() , v.colors[0].rgba);
		v.colors[0].rate = lt["rate"].get<float>();

		ConvertStringToSsColor( rt["rgba"].get<SsString>() , v.colors[2].rgba);
		v.colors[1].rate = rt["rate"].get<float>();

		ConvertStringToSsColor( lb["rgba"].get<SsString>() , v.colors[2].rgba);
		v.colors[2].rate = lb["rate"].get<float>();

		ConvertStringToSsColor( rb["rgba"].get<SsString>() , v.colors[3].rgba);
		v.colors[3].rate = rb["rate"].get<float>();

	}else{
		SsHash color = key->value["color"].get<SsHash>();
		ConvertStringToSsColor( color["rgba"].get<SsString>() , v.color.rgba);
		v.color.rate = color["rate"].get<float>();
	}

}

void	GetSsVertexAnime( const SsKeyframe* key , SsVertexAnime& v )
{
	const SsString& sLT = key->value["LT"].get<SsString>();
	const SsString& sRT = key->value["RT"].get<SsString>();
	const SsString& sLB = key->value["LB"].get<SsString>();
	const SsString& sRB = key->value["RB"].get<SsString>();
	
	StringToPoint2( sLT , v.offsets[0] );
	StringToPoint2( sRT , v.offsets[1] );
	StringToPoint2( sLB , v.offsets[2] );
	StringToPoint2( sRB , v.offsets[3] );

}


void	GetSsRefCell( const SsKeyframe* key , SsRefCell& v )
{
	int id = key->value["mapId"].get<int>();
	SsString name = key->value["name"].get<SsString>();

	v.mapid = id;
	v.name = name;
}


void	GetSsUserDataAnime( const SsKeyframe* key , SsUserDataAnime& v )
{
	v.integer = 0;
	v.point.x = v.point.y = 0;
	v.rect.x = v.rect.y = v.rect.w = v.rect.h = 0; 
	v.string = "";
	v.useInteger = key->value.IsExistHashkey("integer");
	v.usePoint = key->value.IsExistHashkey("point");
	v.useRect = key->value.IsExistHashkey("rect");
	v.useString = key->value.IsExistHashkey("string");

	if ( v.useInteger )
	{
		v.integer = key->value["integer"].get<int>();
	}

	if ( v.usePoint )
	{
		const SsString& str = key->value["point"].get<SsString>();
		StringToPoint2( str , v.point );
	}
	
	if ( v.useRect )
	{
		const SsString& str = key->value["rect"].get<SsString>();
		StringToIRect( str , v.rect );
	}

	if ( v.useString )
	{
		const SsString& str = key->value["string"].get<SsString>();
		v.string = str;
	}

}

void	GetSsInstparamAnime( const SsKeyframe* key , SsInstanceAttr& v )
{
}

#pragma warning( pop )
