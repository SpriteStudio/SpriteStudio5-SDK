//
//	Windows / OSX　共通で呼び出せるファイル選択ダイアログライブラリ
//
//  XPFileOpenDlg.h
//  sssdk
//
//  Created by kurooka on 2013/07/18.
//
//

#ifndef __XP_FILEOPENDLG__
#define __XP_FILEOPENDLG__

class XPFileOpenDlg {
private:
	char*	pszChar;
public:
    XPFileOpenDlg() : pszChar(0) {}
    virtual ~XPFileOpenDlg()
	{
		if ( pszChar )
			delete [] pszChar;
	}
    bool    Show();

	const char* getFilePath(){ return pszChar;}

};

#endif
