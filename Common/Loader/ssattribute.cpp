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
