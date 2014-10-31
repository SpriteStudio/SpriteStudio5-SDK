- 概要
  plistConverter.exeはSprite Studio 5 のスプライトシート機能で出力されたセル情報(plist)を
  PlayStationMobile SDK で使用しやすいxml形式に変換するコンバータです。

  plistConverter.zip			windows用の実行ファイル
  plistConverter_src.zip		ソースファイル Visual Studio Express 2013 for Windows Desktopのプロジェクトファイル
  textuerpack.zip				xmlからアニメを表示する PlayStationMobile SDK のサンプルプログラム

- 使い方
  - Sprite Studio 5でスプライトシートの作成
  メニュー＞OPTPiX Labs＞スプライトシートを生成を選択します。
  Trim Transparentをチェックします。
  Output FormatをCocos2d-xにします。
  TextureSize等を設定し、Exportボタンを押します。
  plistとpngが出力されます。

  - plistのコンバート
  コマンドプロンプトを起動し、plistConverter.exe を実行します。
  第一引数に plist のパスを設定します。
  同じフォルダにxmlファイルが作成されます。

  - ゲームへの組み込み
  作成したxmlファイルとpngファイルをプロジェクトへ追加します。
  サンプルプログラムに付属している UnifiedTexture.cs を参考にxmlファイルを解析し表示を行ってください。






