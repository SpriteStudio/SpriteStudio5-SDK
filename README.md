SpriteStudio5-SDK
=================

このリポジトリについて
-----------------
このリポジトリでは、OPTPiX SpriteStudioで作成したデータをOpenGLで表示するためのビューアサンプル、
アニメーションファイル(sspj、ssae、ssce）の読み込みライブラリ、変換ライブラリ（予定）を公開しています。

ファイルのローダーについては、フォルダ"loader"、ビューア―サンプルについては"viewer_sample"を参照下さい。

また、XMLファイルフォーマットにつきましては、documentフォルダにhtml形式でのドキュメントがありますのでそちらを別途参照下さい。

viewer_sampleについては、win32の実行形式及びOSXコマンドラインアプリとしてbinフォルダにzipで格納を行っております。


ビルド方法について
-----------------
本ソースのビルドには以下のライブラリが必要です。
実行時にも下記DLLが必要な場合がありますので、実行形式と同じフォルダに配置して下さい。
viewer_sampleでは本体ソースコードのcmake実行よりも先に下記OpenGL系ライブラリの導入が必要となります。

下記、ライブラリを導入後、SDKのルートフォルダでcmakeの実行を行うと各プラットフォーム毎にビルドが行えるソリューションファイル等が作成されます。

ただしOPTPiX SpriteStudioのsspj、ssce、ssaeの各XMLファイルの読み込みについては、下記のOpenGL系ライブラリの導入は必要なく、cmakeの実行でビルドが可能です。


Windows/MacOSX共通
---
CMake - http://www.cmake.org/　

2.8.11でビルドを確認済み

Windows
---
- glew  
http://glew.sourceforge.net/

1.10.0でのビルドと動作を確認しています。
Windowsの場合のみ導入が必要です。
下記にビルド済みバイナリもあります。
  
- glfw3	
http://www.glfw.org/

3.0.1での動作を確認しています。
Windows版ではビルド済みバイナリがあります。
	
- anttweakbar
http://anttweakbar.sourceforge.net/doc/tools:anttweakbar:download

1.16で動作を確認しています。
Windows版ではビルド済みのバイナリがあります。


MacOSX
---
- glfw3	
http://www.glfw.org/

http://www.glfw.org/download.html

downloadページでSource archiveからソースをダウンロードして下さい。
cmake用のCmakeListがありますので、cmakeを実行してxcodeもしくは
gcc用のmakeファイルを作成してください。
その後、xcodeであれば、ALLBUILD、Installのプロジェクトを実行してください。

gccの場合は、make installを実行ください。
MacOS Lion以降では、makeを実行するためには、
”Command Line Tools for Xcode”を導入する必要があります。

導入は、XCodeの[Preference] - [Downloads] - [Components] から導入可能です。

Installが実行されると、/usr/local/lib /usr/local/includeへコピーがおこなわれます。

- anttweakbar

Macでは、Brewを使うのが手っ取り早いです。
	下記リポジトリに導入方法がありますのでご参照ください。

https://github.com/davidcox/AntTweakBar

Mac標準ではwgetができないため、これも先にbrewで導入下さい。


ビルド確認環境
---

- Windows7 Visual Studio 2008 2010 (express)
- MacOSX 10.8.4 XCode Version 4.6.3 (4H1503)


