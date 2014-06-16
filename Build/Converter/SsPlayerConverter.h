//
//  SsPlayerConverter.h
//

#ifndef _SsPlayerConverter_h
#define _SsPlayerConverter_h

enum SsPlayerConverterResultCode
{
    SSPC_SUCCESS = 0,                   // 成功
    SSPC_INTERNAL_ERROR,                // 内部エラーが発生しました（※バグなどの不具合が無い限り通常発生しません）
    SSPC_NOT_SPECIFY_INPUT_FILE,        // 入力ファイルの指定がありません
    SSPC_NOT_EXIST_INPUT_FILE,          // 指定された入力ファイルが存在しません
    SSPC_ILLEGAL_ARGUMENT,              // 引数が不正です
    SSPC_NOT_SUPPORT_OUTPUT_ENCODING,   // 対応していない出力ファイルのエンコーディング形式が指定されました
    SSPC_IMPOSSIBLE_PLURAL_SSF_FILES,   // ssfファイルが複数指定されています。一つのみ指定可能です
    SSPC_SSAX_PARSE_FAILED,             // ssaxファイルの解析に失敗しました
    
    END_OF_SSPC
};

#endif
