/*
 * @概要		共通リソース管理クラス
 *
 * $Id: $
 *
 * @author		諒
 * @since		2014/07/18
 */

#pragma once

#include "ICCSInterface.h"

class CGlobalObject:public CCS::ICCSInterface
{
public:
	CGlobalObject( void ):
		m_nRefCount( 1 )
	{
		if( g_pThis == NULL ){
			g_pThis = this;
		}
	}
	virtual ~CGlobalObject( void ){}

public:
	static CGlobalObject* GetThis( void ){ return g_pThis; }

	HINSTANCE GetInstance( void ){ return m_hInstance; }
	void SetInstance( HINSTANCE hInstance ){ m_hInstance = hInstance; }

public:
	virtual ULONG STDCALLMETHOD AddRef( void )
	{
		return ++m_nRefCount;
	}
	virtual ULONG STDCALLMETHOD Release( void )
	{
		if( --m_nRefCount == 0 ){
			delete this;
			return 0;
		}
		return m_nRefCount;
	}
	virtual ULONG STDCALLMETHOD GetRefCount( void )
	{
		return m_nRefCount;
	}

private:
	int m_nRefCount;
	HINSTANCE m_hInstance;
	static CGlobalObject* g_pThis;
};
