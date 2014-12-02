/* PlayStation(R)Mobile SDK 2.00.00
 * Copyright (C) 2014 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

using System;
using System.Collections.Generic;
using System.IO;

using Sce.PlayStation.Core;
using Sce.PlayStation.Core.Graphics;
using Sce.PlayStation.Core.Imaging;
using Sce.PlayStation.Core.Environment;
using Sce.PlayStation.Core.Input;
using Sce.PlayStation.Core.Audio;
using Sce.PlayStation.HighLevel.UI;

namespace ss
{
	public class AppMain
	{
		private static GraphicsContext graphics;		
		
		//アニメ操作用
		private static GamePadData gamePadData;
		private static int frame_count;
		private static bool anime_stop;
		private static bool press;
		
		private static Label label1;
		private static Label label2;

		//xmlから作成したモーションデータ
		private static Player player;
		
		public static void Main (string[] args)
		{
			Initialize ();

			while (true) {
				SystemEvents.CheckEvents ();
				Update ();
				Render ();
			}
		}
		
		public static void Initialize ()
		{
			//グラフィックコンテキストの作成
			graphics = new GraphicsContext( 960, 544, PixelFormat.Rgba, PixelFormat.Depth16, MultiSampleMode.None );		
			
			//プレイヤーの作成とssbxの読み込み
			player = new Player(graphics);
			player.Load("/Application/resources/character_template1.xml");
			
			//プレイヤーの設定
			player.SetPostion(500, 400);		//位置
			player.SetRotation(0);				//角度
			player.SetScale(0.5f, 0.5f);		//スケール
			player.SetAlpha(255);				//透明度
			
			//モーションを指定して再生します。
			player.Play("character_template_3head/stance");
			
			String str;
			//UIの初期化
            UISystem.Initialize(graphics);
			//シーンの生成
            Scene scene=new Scene();
            UISystem.SetScene(scene,null);
			//ラベル作成
			label1=new Label();
			str = string.Format("xml load:{0}", player.GetLoadtime( 0 ));
            label1.Text=str;
            label1.TextColor=new UIColor(1.0f,1.0f,1.0f,1.0f);
			label1.SetPosition(0.0f, 0.0f);
            scene.RootWidget.AddChildLast(label1);			

			label2=new Label();
			str = string.Format("tex load:{0}", player.GetLoadtime( 1 ));
            label2.Text=str;
            label2.TextColor=new UIColor(1.0f,1.0f,1.0f,1.0f);
			label2.SetPosition(0.0f, 20.0f);
            scene.RootWidget.AddChildLast(label2);			
	}
	
		public static void Update ()
		{
			player.Update();
			int maxframe = player.GetMaxFrame();
			
			//アニメーション更新
			if( anime_stop == false )
			{
				frame_count = player.GetFrame(); 
			}
			else
			{
				player.SetFrame(frame_count); 
			}
			
			//キー入力でアニメーションを操作する
			gamePadData = GamePad.GetData(0);
			if((gamePadData.Buttons & GamePadButtons.Left) != 0)
	        {
				if ( press == false )
				{
					frame_count--;
					if ( frame_count < 0 )
					{
						frame_count += maxframe;
					}
				}
				press = true;
	        }
	        else if((gamePadData.Buttons & GamePadButtons.Right) != 0)
	        {
				if ( press == false )
				{
					frame_count++;
					if ( frame_count >= maxframe )
					{
						frame_count -= maxframe;
					}
				}
				press = true;
	        }
	        else if((gamePadData.Buttons & GamePadButtons.Up) != 0)
	        {
				if ( press == false )
				{
					frame_count += 10;
					if ( frame_count >= maxframe )
					{
						frame_count -= maxframe;
					}
				}
				press = true;
	        }
	        else if((gamePadData.Buttons & GamePadButtons.Down) != 0)
	        {
				if ( press == false )
				{
					frame_count -= 10;
					if ( frame_count < 0 )
					{
						frame_count += maxframe;
					}
				}
				press = true;
	        }			
	        else if((gamePadData.Buttons & GamePadButtons.Circle) != 0)
	        {
				if ( press == false )
				{
					if ( anime_stop == false )
					{
						anime_stop = true;
						player.Stop();	//アニメ停止
					}
					else
					{
						anime_stop = false;
						player.Resume();	//アニメ再生
					}
					
				}
				press = true;
	        }			
			else
			{
				press = false;
			}
			
			//タッチ情報の取得
            List<TouchData> touchData=Touch.GetData(0);
            
            //UIの更新
            UISystem.Update(touchData);			//UIの更新
		}

		public static void Render ()
		{
			graphics.Clear();

			//プレイヤー描画
			player.Draw();

			//UIの描画	
            UISystem.Render();
			
			//画面更新
			graphics.SwapBuffers();	
		}
	}
}
