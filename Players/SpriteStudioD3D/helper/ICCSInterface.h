/*
 * @概要		基底インターフェース用ヘッダ
 *
 * $Id: 61e69bf30650f25e9f151f1eca7e0a6b977a5169 $
 *
 * @author		諒
 * @since		2007/05/02
 */

#pragma once
#define STDCALLMETHOD __stdcall

#ifndef _CCS_INTERFACE_
#define _CCS_INTERFACE_
namespace CCS{

// ======================================================================
//  基本インターフェース定義
// ======================================================================
class ICCSInterface:public IUnknown{
public:
	virtual ULONG STDCALLMETHOD AddRef( void ) = 0;
	virtual ULONG STDCALLMETHOD Release( void ) = 0;
	virtual HRESULT STDCALLMETHOD QueryInterface( REFIID riid, void **ppvObject ){ return E_NOINTERFACE; }
	virtual ULONG STDCALLMETHOD GetRefCount( void ) = 0;
};

}	// End of namespace CCS
#endif
