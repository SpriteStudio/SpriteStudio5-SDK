#ifndef _WIN32

//
//  OSXFileOpen.m
//  sssdk
//
//  Created by kurooka on 2013/07/18.
//
//

#import "OSXFileOpen.h"


#import <AppKit/NSOpenPanel.h>
#import <Foundation/Foundation.h>


extern void GlContexMakeCurrent();

bool MacOSXFileOpenDlg::show()
{
    NSOpenPanel *openPanel	= [NSOpenPanel openPanel];
    NSArray *allowedFileTypes = [NSArray arrayWithObjects:@"sspj",@"'sspj'",nil];
    //  NSOpenPanel interface has changed since Mac OSX v10.6.
    [openPanel setAllowedFileTypes:allowedFileTypes];
    NSInteger pressedButton = [openPanel runModal];
    
    if( pressedButton == NSOKButton ){
        
        // get file path
        NSURL * filePath = [openPanel URL];
        NSString *urlString = [filePath path];
        // open file here
        NSLog(@"file opened '%@'", filePath);
                
        filepath = (char*)[urlString UTF8String];//形式を変換
        GlContexMakeCurrent();
        return true;
    }else if( pressedButton == NSCancelButton ){
     	NSLog(@"Cancel button was pressed.");
    }else{
     	// error
    }

    GlContexMakeCurrent();
    return false;
}




#endif
