/*
 * @概要		プリコンパイル済みヘッダー
 *
 * $Id: $
 *
 * @author		諒
 * @since		2014/07/18
 */

#pragma once

//RELEASE実行の場合は、vectorの範囲外チェックを無効にする
#undef _SECURE_SCL
#ifndef _DEBUG
#define _SECURE_SCL 0
#else
#define _SECURE_SCL 1
#endif

// VC2005以降ででるワーニングを抑止
#define _CRT_SECURE_NO_WARNINGS 1

// 不要なヘッダを取り除く
#define WIN32_LEAN_AND_MEAN

#define DLL_EXPORT 

// システムヘッダの定義
#include <Windows.h>
#include <Process.h>
#include <mbctype.h>
#include <conio.h>

#include <string>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <list>
#include <deque>

#include <unknwn.h>
#include <atlbase.h>

#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )
