#include "DxLib.h"
#include "SSPlayer\SS5Player.h"

static int previousTime;
static int waitTime;
int mGameExec;

#define WAIT_FRAME (16)

void init(void);
void update(float dt);
void relese(void);

/// SS5プレイヤー
ss::Player *ssplayer;
ss::ResourceManager *resman;

/**
* メイン関数
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//DXライブラリの初期化
	ChangeWindowMode(true);	//ウインドウモード
	SetGraphMode(1280, 720, GetColorBitDepth() );
	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}
	SetDrawScreen(DX_SCREEN_BACK);

	//メインループ
	mGameExec = 1;
	previousTime = GetNowCount();
	
	/// プレイヤー初期化
	init( );
	
	while(mGameExec == 1){
		ClearDrawScreen();
		update((float)waitTime / 1000.0f );
		ScreenFlip();
		waitTime = GetNowCount() - previousTime;
		previousTime = GetNowCount();

		if (waitTime < WAIT_FRAME){
			WaitTimer((WAIT_FRAME - waitTime));
		}else{
			if(ProcessMessage() == -1) mGameExec = 0;
		}
	}

	/// プレイヤー終了処理
	relese( );


	DxLib_End();			// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}

void init( void )
{
	/**********************************************************************************

	SSアニメ表示のサンプルコード
	Visual Studio Express 2013 for Windows Desktop、DXライブラリで動作を確認しています。
	ssbpとpngがあれば再生する事ができますが、Resourcesフォルダにsspjも含まれています。

	**********************************************************************************/

	//リソースマネージャの作成
	resman = ss::ResourceManager::getInstance();
	//プレイヤーの作成
	ssplayer = ss::Player::create();

	//アニメデータをリソースに追加

	//それぞれのプラットフォームに合わせたパスへ変更してください。
	resman->addData("character_template_comipo\\character_template1.ssbp");
	//プレイヤーにリソースを割り当て
	ssplayer->setData("character_template1");        // ssbpファイル名（拡張子不要）
	//再生するモーションを設定
	ssplayer->play("character_template_3head/stance");				 // アニメーション名を指定(ssae名/アニメーション名も可能、詳しくは後述)

	//表示位置を設定
	ssplayer->setPosition(1280/2, 600);
	//スケール設定
	ssplayer->setScale(0.5f, 0.5f);
	//回転を設定
	ssplayer->setRotation(0.0f, 0.0f, 0.0f);
	//透明度を設定
	ssplayer->setAlpha(255);

}

//メインループ
//Zボタンでアニメをポーズ、再開を切り替えできます。
//ポーズ中は左右キーで再生するフレームを変更できます。
bool push = false;
int count = 0;
bool pause = false;
void update(float dt)
{

	int animax = ssplayer->getMaxFrame();
	if (CheckHitKey(KEY_INPUT_ESCAPE))
	{
		mGameExec = 0;
	}

	if (CheckHitKey(KEY_INPUT_Z))
	{
		if (push == false )
		{
			if (pause == false )
			{
				pause = true;
				count = 0;
				ssplayer->pause();
			}
			else
			{
				pause = false;
				ssplayer->resume();
			}
		}
		push = true;

	}
	else if (CheckHitKey(KEY_INPUT_UP))
	{
		if (push == false)
		{
			count += 20;
			if (count >= animax)
			{
				count = 0;
			}
		}
		push = true;
	}
	else if (CheckHitKey(KEY_INPUT_DOWN))
	{
		if (push == false)
		{
			count -= 20;
			if (count < 0)
			{
				count = animax - 1;
			}
		}
		push = true;
	}
	else if (CheckHitKey(KEY_INPUT_LEFT))
	{
		if (push == false)
		{
			count--;
			if (count < 0)
			{
				count = animax-1;
			}
		}
		push = true;
	}
	else if (CheckHitKey(KEY_INPUT_RIGHT))
	{
		if (push == false)
		{
			count++;
			if (count >= animax)
			{
				count = 0;
			}
		}
		push = true;
	}
	else
	{
		push = false;
	}

	if (pause == true)
	{
		ssplayer->setFrameNo(count % animax);
	}

	//アニメーションのフレームを表示
	char str[128];
	sprintf(str, "play:%d frame:%d", (int)pause, count );
	DrawString(100, 100, str, GetColor(255, 255, 255));

	//プレイヤーの更新、引数は前回の更新処理から経過した時間
	ssplayer->update(dt);
	//プレイヤーの描画
	ssplayer->draw();

}

/**
* プレイヤー終了処理
*/
void relese( void )
{

	//テクスチャの解放
	resman->releseTexture("character_template1");
	//SS5Playerの削除
	delete (ssplayer);	
	delete (resman);
}









