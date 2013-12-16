//
//  OSXFileOpen.h
//  sssdk
//
//  Created by kurooka on 2013/07/18.
//
//

#ifndef _WIN32

class MacOSXFileOpenDlg
{
private:
    char* filepath;
    
public:
    MacOSXFileOpenDlg() : filepath(0){}
    virtual ~MacOSXFileOpenDlg(){}
    
    bool show();
    
    const char* getFilePath()
    {
        return filepath;
    }
};

#endif

//void    testOpenDlg();
