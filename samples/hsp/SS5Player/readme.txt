・SS5Player for HSP
■概要
	Sprite Studio 5 で作成したアニメーションをHSPで再生するサンプルプログラムです。
	
	SpriteStudio5-SDK(https://github.com/SpriteStudio/SpriteStudio5-SDK)に含まれる
	Ss5Converterを使用してsspjからsspbを作成します。
	
	作成したssbpと画像ファイルをゲームのリソースフォルダに置きSS5PlayerDllを使用して読み込みます。
	SS5PlayerDllからアニメーションのステータスを取得してHSP側で表示を行います。
	
	DLL内部には32個のアニメプレイヤーを持っており、プレイヤー番号を指定して各プレイヤーを操作します。
	プレイヤー数を増やす場合はSS5PlayerDll.cppに定義されているSSPLAYER_MAXを変更してください。
	
	※サンプルプログラムの制限
	未対応アトリビュート
	　X回転、Y回転、カラーブレンド、頂点変形、Xサイズ(出力しているがサンプルプログラムが未対応)、Yサイズ(出力しているがサンプルプログラムが未対応)
	　UXX移動、UVY移動、UV回転、UXXスケール、UVYスケール、ユーザーデータ、インスタンスアニメ
	サンプルではパーツのブレンド方法には対応していません。
	データは出力しているので、必要であれば各自対応してください。

	サンプルでは32bitのpngを使用していますが、16bit等の画像を使用しHSPの標準命令で表示すれば動作速度は格段に早くなると思います。
	必要に応じてステータスから表示部を作成してください。

	pngの表示にArtlet2Dを使用しています。

	SS5PlayerDll.dllはVisual Studio Express for Windows Desktopで作成されています。
	必要に合わせてDLL側に関数を追加してください。

■SS5PlayerDll.dll関数リファレンス

	#cfunc SSPlayer_initlize			"SSPlayer_initlize"
		・機能
			SS5PlayerDll.dllの初期化します
			使用前に必ず呼び出してください。

		・引数
			なし

		・戻り値
			常に0

	#cfunc SSPlayer_setFps				"SSPlayer_setFps"				int
		・機能
			SSPlayer_updateを呼び出したときにアニメが進む速度を指定します。
			60FPSで動作する場合は60を設定してください。

		・引数
			int 	：FPS

		・戻り値
			常に0

	#cfunc SSPlayer_addData				"SSPlayer_addData"				str 
		・機能
			ssbpファイルを読み込みDLL内部に保持します。
			複数のssbpファイルを読み込み保持することができます。
			存在しないファイルを指定するとエラー1が発生します。

		・引数
			str 	：ssbpファイルのパス

		・戻り値
			常に0

	#cfunc SSPlayer_getPlayerNum		"SSPlayer_getPlayerNum"		 
		・機能
			DLLで同時に制御できるプレイヤーの数を取得します。
			32個まで同時にアニメを再生する事ができます。

		・引数
			なし

		・戻り値
			同時に制御できるプレイヤー数

	#cfunc SSPlayer_get_texureIndex		"SSPlayer_get_texureIndex" 		int
		・機能
			読み込んだsspbで使用しているテクスチャ番号を取得します。
			テクスチャ番号はDLL内で割り当てた番号となり、描画を行うときにこの番号で表示する画像を決定します。
			100枚までテクスチャデータに保持する事ができ、引数で0番から順にテクスチャ番号を取得してください。
			テクスチャ番号は0番から割り当ります、取得した番号のスクリーンに対応する画像を読み込んでください。

		・引数
			int 	：DLLで保持しているテクスチャデータの番号（0～99）

		・戻り値
			テクスチャに割り当たった番号（0～99）

	#cfunc SSPlayer_get_texureName		"SSPlayer_get_texureName"		int,sptr
		・機能
			読み込んだsspbで使用しているテクスチャ名を取得します。
			DLLで保持しているテクスチャデータからファイル名を取得します。
			100枚まで保持する事ができ、引数で0番から順にテクスチャ名を取得してください。
			SSPlayer_get_texureIndexで取得した番号のスクリーンにSSPlayer_get_texureNameで取得した画像を読み込んでください。

		・引数
			int 	：DLLで保持しているテクスチャデータの番号（0～99）
			pstr	：テクスチャに割り当たったファイル名

		・戻り値
			常に0

	#cfunc SSPlayer_play				"SSPlayer_play"					int,str,str 
		・機能
			アニメーションを再生します。
			sspb名（拡張子なし）とssae名/モーション名で指定します。
			先にSSPlayer_addDataでssbpを読み込んでおく必要があります。
			指定したssbpがDLLに読み込まれていない場合、エラー１が発生します。
			再生中にSSPlayer_playを呼び出すことで、同じプレイヤーで別のアニメを再生する事ができます。

		・引数
			int		：プレイヤー番号（0～31）
			str		：ssbp名（拡張子なし）
			str		：ssae名/モーション名（間に/を入れて指定してください。）

		・戻り値
			常に0

	#cfunc SSPlayer_update				"SSPlayer_update"			 
		・機能
			DLLで保持している32個のプレイヤーのステータスを更新します。
			SSPlayer_update後にSSPlayer_get_partStatusでパーツのステータスを取得し、
			ステータスの内容で描画を行うのが基本の流れになります。

		・引数
			なし

		・戻り値
			常に0

	#cfunc SSPlayer_get_partNum			"SSPlayer_get_partNum"			int 
		・機能
			アニメに含まれるパーツ数を取得します。
			パーツ数はパーツステータスを取得する場合に使用します。

		・引数
			int		：プレイヤー番号（0～31）

		・戻り値
			アニメに含まれるパーツ数

	#cfunc SSPlayer_get_partStatus		"SSPlayer_get_partStatus"		int,int,var 
		・機能
			パーツのステータスを取得します。
			パーツは優先度順にソートされており、0番のパーツが一番奥になります。
			SSPlayer_get_partNumで取得したパーツ数分ループしてステータスを取得してください。

		・引数
			int		：プレイヤー番号（0～31）
			int 	：パーツ番号
			var		：ステータスを受け取る配列のポインタ（dim partstate, 18）
						 0：テクスチャ番号
						 1：非表示フラグ
						 2：表示位置X
						 3：表示位置Y
						 4：テクスチャ内の開始位置X
						 5：テクスチャ内の開始位置Y
						 6：画像の幅
						 7：画像の高さ
						 8：拡大率X
						 9：拡大率Y
						10：回転角度
						11：透明度
						12：ブレンドモード
						13：横反転フラグ
						14：縦反転フラグ
						15：Xサイズ
						16：Yサイズ
						17：当たり半径

		・戻り値
			常に0

	#cfunc SSPlayer_setPosition			"SSPlayer_setPosition"			int,int,int 
		・機能
			プレイヤーの位置を設定します。

		・引数
			int		：プレイヤー番号（0～31）
			int 	：表示位置X
			int		：表示位置Y

		・戻り値
			常に0

	#cfunc SSPlayer_setRotation			"SSPlayer_setRotation"			int,double,double,double 
		・機能
			プレイヤーの回転を設定します。

		・引数
			int		：プレイヤー番号（0～31）
			double	：X回転角度（未対応ですが内部的には計算されているので、0以外を設定すると表示が崩れます）
			double	：Y回転角度（未対応ですが内部的には計算されているので、0以外を設定すると表示が崩れます）
			double	：Z回転角度（0～360）

		・戻り値
			常に0

	#cfunc SSPlayer_setScale			"SSPlayer_setScale"				int,double,double 
		・機能
			プレイヤーの拡大率を設定します。

		・引数
			int		：プレイヤー番号（0～31）
			double	：X拡大率（1.0＝等倍）
			double	：Y拡大率（1.0＝等倍）

		・戻り値
			常に0

	#cfunc SSPlayer_setAlpha			"SSPlayer_setAlpha"				int,int 
		・機能
			プレイヤーの透明度を設定します。

		・引数
			int		：プレイヤー番号（0～31）
			int		：透明度（0～255）

		・戻り値
			常に0

	#cfunc SSPlayer_setFlip				"SSPlayer_setFlip"				int,int,int 
		・機能
			プレイヤーの反転を設定します。

		・引数
			int		：プレイヤー番号（0～31）
			int		：X反転（0反転なし、0以外反転）
			int		：Y反転（0反転なし、0以外反転）

		・戻り値
			常に0

	#cfunc SSPlayer_pause				"SSPlayer_pause"				int 
		・機能
			プレイヤーのアニメ再生を一時停止します。
			SSPlayer_pauseで一時停止したプレイヤーはSSPlayer_resumeで再開する事ができます。
			一時停止状態で、SSPlayer_setFrameNoを行い再生フレームを指定するとゲーム側で表示フレームを管理する事ができます。

		・引数
			int		：プレイヤー番号（0～31）

		・戻り値
			常に0

	#cfunc SSPlayer_resume				"SSPlayer_resume"				int 
		・機能
			プレイヤーのアニメ再生を再開します。
			SSPlayer_pauseで一時停止したプレイヤーはSSPlayer_resumeで再開する事ができます。

		・引数
			int		：プレイヤー番号（0～31）

		・戻り値
			常に0

	#cfunc SSPlayer_setFrameNo			"SSPlayer_setFrameNo"			int,int 
		・機能
			アニメの再生位置を設定します。

		・引数
			int		：プレイヤー番号（0～31）
			int		：再生を行うフレーム

		・戻り値
			常に0

	#cfunc SSPlayer_getMaxFrame			"SSPlayer_getMaxFrame"			int 
		・機能
			アニメの総フレームを取得します。

		・引数
			int		：プレイヤー番号（0～31）

		・戻り値
			アニメの総フレーム

	#cfunc SSPlayer_getFrameNo			"SSPlayer_getFrameNo"			int 
		・機能
			現在再生しているフレームを取得します。

		・引数
			int		：プレイヤー番号（0～31）

		・戻り値
			現在再生しているフレーム

	#cfunc SSPlayer_get_namePartStatus	"SSPlayer_get_namePartStatus"	int,str,int,var 
		・機能
			パーツ名からパーツステータスを取得します。
			再生しているアニメに存在しないパーツ名をしていした場合、テクスチャ番号が-1になります。
			座標やサイズを取得してエフェクトを出したりコリジョンとして使用が考えられます。

		・引数
			int		：プレイヤー番号（0～31）
			str		：パーツ名
			int		：取得するフレーム（-1で現在のフレームのステータスを取得する）
			var		：ステータスを受け取る配列のポインタ（dim partstate, 18）

		・戻り値
			常に0

	#cfunc SSPlayer_Delete				"SSPlayer_Delete"				 
		・機能
			DLL内で保持している情報を解放します。
			アプリケーションの終了時に呼び出してください。
			画像の解放はゲーム側で行ってください。

		・引数
			なし

		・戻り値
			常に0





