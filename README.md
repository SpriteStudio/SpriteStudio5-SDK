OPTPiX SpriteStudio 5 SDK
=================

はじめに
-----------------
本SDKは、OPTPiX SpriteStudio 5で製作したデータをユーザーの皆様の再生環境に応じて
柔軟にご利用いただくためのライブラリ、およびツール群で構成されます。

- ローダー：SpriteStudio 5 形式のファイル(sspj、ssae、ssce）の読み込みモジュール
- アニメータ：座標変換、補間計算などのアニメーションモジュール
- ドローワー：OpenGL/DirectX を利用した描画モジュール

上記モジュールを利用した下記ツールも同梱しています。

- ビューアー：Windows/Mac 用ビューアーサンプル
- コンバータ(※開発中)：SpriteStudio 5 形式のファイルから独自のファイル形式へ変換します。

以上で構成されています。

ファイルのローダーについては、フォルダ"loader"、ビューア―サンプルについては"viewer_sample"を参照下さい。

また、XMLファイルフォーマットにつきましては、documentフォルダにhtml形式でのドキュメントがありますのでそちらを別途参照下さい。

viewer_sampleについては、win32の実行形式及びOSXコマンドラインアプリとしてbinフォルダにzipで格納を行っております。


使用しているオープンソースライブラリについて
----------------
このサンプル、SDKでは以下のライブラリを使用しています。
各ライブラリのライセンス等につきましては、下記ページよりご確認ください。

ローダー部分
- TinyXML2
http://www.grinninglizard.com/tinyxml/
- Babel
http://tricklib.com/cxx/ex/babel/

レンダラー
- OpenGL 
http://www.opengl.org/
- glew ( WIndows )
http://glew.sourceforge.net/

サンプルのGUI部分
- glfw3 
http://www.glfw.org/
- anttweakbar
http://anttweakbar.sourceforge.net/doc/tools:anttweakbar:download




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


