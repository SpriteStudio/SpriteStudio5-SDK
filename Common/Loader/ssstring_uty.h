#ifndef __SSSTRING_UTY__
#define __SSSTRING_UTY__


#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>

/*
 * @brief     文字列を指定のkeyで分割して返します。
 *
 * @param[in] in_str		分割する文字列
 * @param[in] key			分割のキーとなる文字列
 * @param[out] out_array	分割した文字列を格納する文字列リスト
 * @retval なし
*/
void	split_string( const std::string &in_str , 
						const char key, 
						std::vector<std::string>& out_array );

//bool	is_digit_string( std::string &in_str );
bool	is_digit_string( std::string &in_str , bool* is_priod = 0 );

/*
 * @brief ファイルのフルパスからフォルダパスのみを取得します。
 * @param[in] ファイルパス
 * @retval ファイルパスからファイル名を取り除いた文字列
*/
std::string path2dir(const std::string &path);


/*
 * @brief ファイルのフルパスからファイル名のみを取得します。
 * @param[in] ファイルパス
 * @retval ファイルパスからフォルダ名を取り除いた文字列
*/
std::string path2file(const std::string &path);


/*
 * @brief 相対パスを絶対パスへ変換する
 * param[in] basePath 基準ディレクトリ
 * param[int] relPath 相対パス
 * retval relpathを絶対パスへ変換した値
*/
std::string getFullPath( const std::string& basePath , const std::string &relPath);


std::string nomarizeFilename( std::string str );

#endif
