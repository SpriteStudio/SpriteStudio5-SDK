/* PlayStation(R)Mobile SDK 2.00.00
 * Copyright (C) 2014 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

using System;
using System.Collections.Generic;
using System.IO;
using System.Diagnostics;

using Sce.PlayStation.Core;
using Sce.PlayStation.Core.Graphics;
using Sce.PlayStation.Core.Imaging;
using Sce.PlayStation.Core.Environment;
using Sce.PlayStation.Core.Input;
using Sce.PlayStation.Core.Audio;


namespace ss
{
	//リソースマネージャ
	public class ResourceManager
	{
		int loadtime_start;
		int loadtime_end;
		public Dictionary<string, SSBX_ANIMEDATA> animedata;
		
		public ResourceManager()
		{
			animedata = new Dictionary<string, SSBX_ANIMEDATA>();
		}
		//XML読み込み
		public void Load( string ssbx_name )
		{
			//登録用の名前を作成（ファイル名）
			string addname;
			int s = ssbx_name.LastIndexOf("/") + 1;
			int e = ssbx_name.LastIndexOf(".");
			addname = ssbx_name.Substring(s,e-s);
			
			//計測用タイマーの作成
			Stopwatch stopwatch;
			stopwatch = new Stopwatch();
			stopwatch.Start();
			
			//ssbxの読み込み
			loadtime_start = (int)stopwatch.ElapsedMilliseconds;	//時間計測
			if ( animedata.ContainsKey(addname) == false )
			{
				//キーを検索して存在しない場合は読み込みを行う
				animedata.Add (addname, SSBX.GetSsbx_animedata(ssbx_name));

				//画像読み込み
				int idx;
				for ( idx = 0; idx < animedata[addname].texturedata.Count; idx++ )
				{
					string str;
					str = string.Format("texture{0:D3}", idx);
					SSBX_TEXTUREDATA ssbx_texturedata = animedata[addname].texturedata[str];
					int id = ssbx_texturedata.id;
					string name = "/Application/resources/" + ssbx_texturedata.name;
	
					//画像読み込み
					animedata[addname].tex[idx] = new Texture2D(name, false, PixelFormat.Rgba);
				}

			}
			loadtime_end = (int)stopwatch.ElapsedMilliseconds;	//時間計測
		}
		//データの削除
		public void Release( string ssbx_name )
		{
			//該当のデータがある場合は削除する
			if ( animedata.ContainsKey(ssbx_name) == true )
			{
				//テクスチャの解放
				int idx;
				for ( idx = 0; idx < animedata[ssbx_name].texturedata.Count; idx++ )
				{
					if ( animedata[ssbx_name].tex[idx] != null )
					{
						animedata[ssbx_name].tex[idx].Dispose();
						animedata[ssbx_name].tex[idx] = null;
					}
				}
				animedata.Remove(ssbx_name);
			}
		}

		//ロード時間の計測
		public int GetLoadtime( )
		{
			return ( loadtime_end - loadtime_start );
		}
		//アニメデータの取得
		public SSBX_ANIMEDATA GetAnimeData( string ssbx_name )
		{
			if ( animedata.ContainsKey(ssbx_name) == false )
			{
				return ( null );
			}
			return ( animedata[ssbx_name] );
		}
		
	}
	
	//SS5プレイヤー　for PSM
	public class Player
	{
		//描画用
		private GraphicsContext graphics;		
		private VertexBuffer vertexBuffer;
		private Matrix4 screenMatrix;
		private int indexSize = 4;
		private ushort[] indices;	
		private ShaderProgram shaderProgram;
		
		//プレイヤーのステータス
		private float frame_count;	//再生フレーム
		private bool anime_stop;	//再生フラグ
		private int pos_x;			//位置
		private int pos_y;
		private float scale_x;			//スケール
		private float scale_y;
		private float rot_z;			//回転
		private float alpha;			//半透明
	
		
		//xmlから作成したモーションデータ
		private SSBX_ANIMEDATA ssbx_animedata;
		private SSBX_MOTIONDATA ssbx_motiondata;
		
		public Player(GraphicsContext app_graphics)
		{
			graphics = app_graphics;
			ssbx_animedata = null;
			frame_count = 0;
			anime_stop = true;
			
			shaderProgram = new ShaderProgram("/Application/shaders/Sprite.cgx");
		    shaderProgram.SetUniformBinding(0, "u_ScreenMatrix");

			indices = new ushort[indexSize];
			indices[0] = 0;
			indices[1] = 1;
			indices[2] = 2;
			indices[3] = 3;
			
			ImageRect rectScreen = graphics.Screen.Rectangle;
			screenMatrix = new Matrix4(
				 2.0f/rectScreen.Width,	0.0f,	    0.0f, 0.0f,
				 0.0f,   -2.0f/rectScreen.Height,	0.0f, 0.0f,
				 0.0f,   0.0f, 1.0f, 0.0f,
				 -1.0f,  1.0f, 0.0f, 1.0f
			);
			//												vertex pos,               texture,       color
			vertexBuffer = new VertexBuffer(4, indexSize, VertexFormat.Float3, VertexFormat.Float2, VertexFormat.Float4);
		}
		
		//アニメデータの設定
		public void SetAnimedata( ResourceManager res, string name )
		{
			ssbx_animedata = res.GetAnimeData(name);
		}

		//再生
		public void Play( string motionname )
		{
			if ( ssbx_animedata == null )
			{
				return;
			}
			ssbx_motiondata = ssbx_animedata.motiondata[motionname];
			anime_stop = false;
			frame_count = 0;
		}
		//停止
		public void Stop()
		{
			//データの解放
			ssbx_animedata = null;
			ssbx_motiondata = null;
		}
		//一時停止
		public void Pause()
		{
			anime_stop = true;
		}
		//
		public void Resume( )
		{
			anime_stop = false;
		}
		//アニメの総フレームを取得
		public int GetMaxFrame()
		{
			int rc = 0;
			if ( ssbx_motiondata != null )
			{
				rc = ssbx_motiondata.maxframe;
			}
			return rc;
		}
		//再生しているフレームを取得
		public int GetFrame()
		{
			int rc = 0;
			if ( ssbx_motiondata != null )
			{
				rc = (int)frame_count;
			}
			return rc;
		}
		//位置を設定
		public void SetPostion( int x, int y )
		{
			pos_x = x;
			pos_y = y;
		}
		//スケールを設定
		public void SetScale( float x, float y )
		{
			scale_x = x;
			scale_y = y;
		}
		//回転を設定
		public void SetRotation( float z )
		{
			rot_z = z;
		}
		//回転を設定
		public void SetAlpha( int a )
		{
			alpha = (float)a / 255.0f;
		}
		//再生するフレームを設定
		public void SetFrame( int frame )
		{
			if ( ssbx_motiondata != null )
			{
				frame_count = frame;
				if ( frame_count >= ssbx_motiondata.maxframe )
				{
					frame_count = ssbx_motiondata.maxframe - 1;
				}
				if ( frame_count < 0 )
				{
					frame_count = 0;
				}
			}
		}
		//プレイヤーの更新
		public void Update()
		{
			if ( ssbx_motiondata == null )
			{
				return;
			}

			int maxframe = ssbx_motiondata.maxframe;
			
			//アニメーション更新
			if( anime_stop == false )
			{
				frame_count += ( (float)ssbx_motiondata.fps / 60.0f ); 
				if ( (int)frame_count >= maxframe )	//最後まで表示したら先頭に戻す
				{
					frame_count = 0;
				}
			}
		}
		//プレイヤーの描画
		public void Draw()
		{
			if ( ssbx_motiondata == null )
			{
				return;
			}
	
			//描画
			int nowframe = (int)frame_count;

			string str;
			str = string.Format("frame{0:D3}", nowframe);
			SSBX_FRAMEDATA ssbx_framedata = ssbx_motiondata.framedata[str];
			int idx = 0;
			for ( idx = 0; idx < ssbx_framedata.framedata.Count; idx++ )
			{
				str = string.Format("part{0:D3}", idx);
				SSBX_PARTDATA ssbx_partdata = ssbx_framedata.framedata[str];
				PART_STATE state = ssbx_partdata.partdata;
				
				
				//プレイヤーのステータスからマトリクスの変形を行う
/*
				float[] mat = new float[16];
				IdentityMatrix(mat);
				TranslationMatrixM(mat, state.x, state.y, 0 );
				TranslationMatrixM(mat, pos_x, pos_y, 0 );
				RotationXYZMatrixM(mat, 0.0f, 0.0f, rot_z);
				ScaleMatrixM(mat,scale_x,scale_y,1.0f);
*/				
				//最終的な表示座標を取得
				state.x = pos_x + (state.x * scale_x);
				state.y = pos_y + (state.y * scale_y);
				
				if ( ( scale_x * scale_y ) < 0 )
				{
					state.rotz = -state.rotz;
				}
				
				if ( rot_z != 1.0f )
				{
					state.rotz += rot_z;
					get_rotation(ref state.x, ref state.y, pos_x, pos_y, rot_z);
				}
				
				//スプライトの描画
				sprite_draw( state );
			}
		}
		
		private void sprite_draw( PART_STATE state )
		{
			if ( state.tex_id == -1 )
			{
				//NULLパーツ
				return;
			}
			float tex_w = ssbx_animedata.tex[state.tex_id].Width;
			float tex_h = ssbx_animedata.tex[state.tex_id].Height;
			float u1 = (float)state.rect_x / tex_w;
			float v1 = (float)state.rect_y / tex_h;
			float u2 = (float)( state.rect_x + state.rect_w ) / tex_w;
			float v2 = (float)( state.rect_y + state.rect_h ) / tex_h;
			
			float[] texcoords = {
			    u1, v1, // 0 top left.
			    u1, v2, // 1 bottom left.
			    u2, v1, // 2 top right.
			    u2, v2, // 3 bottom right.
			};
			if ( ( state.flip_x != 0 ) && ( state.flip_y != 0 ) )
			{
				texcoords[0] = u2;
				texcoords[1] = v2;
				texcoords[2] = u2;
				texcoords[3] = v1;
				texcoords[4] = u1;
				texcoords[5] = v2;
				texcoords[6] = u1;
				texcoords[7] = v1;
			}
			else if ( state.flip_x != 0 )
			{
				texcoords[0] = u2;
				texcoords[1] = v1;
				texcoords[2] = u2;
				texcoords[3] = v2;
				texcoords[4] = u1;
				texcoords[5] = v1;
				texcoords[6] = u1;
				texcoords[7] = v2;
			}
			else if ( state.flip_y != 0 )
			{
				texcoords[0] = u1;
				texcoords[1] = v2;
				texcoords[2] = u1;
				texcoords[3] = v1;
				texcoords[4] = u2;
				texcoords[5] = v2;
				texcoords[6] = u2;
				texcoords[7] = v1;
			}
			
			float[] colors = {
			    1.0f,   1.0f,   1.0f,   state.alpha * alpha,   // 0 top left.
			    1.0f,   1.0f,   1.0f,   state.alpha * alpha,   // 1 bottom left.
			    1.0f,   1.0f,   1.0f,   state.alpha * alpha,   // 2 top right.
			    1.0f,   1.0f,   1.0f,   state.alpha * alpha,   // 3 bottom right.
			};
		
			float w = ( state.rect_w * state.scale_x * scale_x );
			float h = ( state.rect_h * state.scale_y * scale_y );
			float x = (float)state.x - ( w / 2.0f );
			float y = (float)state.y - ( h / 2.0f );
			
			
			float[] vertices=new float[12];
			vertices[0]= x;   // x0
			vertices[1]= y;   // y0
			vertices[2]= 0.0f;   // z0
			get_rotation(ref vertices[0], ref vertices[1], (float)state.x, (float)state.y, state.rotz);
			
			vertices[3]= x;   // x1
			vertices[4]= y + h; // y1
			vertices[5]= 0.0f;   // z1
			get_rotation(ref vertices[3], ref vertices[4], (float)state.x, (float)state.y, state.rotz);
			
			vertices[6]= x + w;  // x2
			vertices[7]= y;   // y2
			vertices[8]= 0.0f;   // z2
			get_rotation(ref vertices[6], ref vertices[7], (float)state.x, (float)state.y, state.rotz);
			
			vertices[9]= x + w;  // x3
			vertices[10]= y + h;    // y3
			vertices[11]= 0.0f;  // z3	
			get_rotation(ref vertices[9], ref vertices[10], (float)state.x, (float)state.y, state.rotz);
			
			
			vertexBuffer.SetVertices(0, vertices);
			vertexBuffer.SetVertices(1, texcoords);
			vertexBuffer.SetVertices(2, colors);
			
			vertexBuffer.SetIndices(indices);
			graphics.SetVertexBuffer(0, vertexBuffer);
			
			graphics.SetShaderProgram(shaderProgram);
			graphics.SetTexture(0, ssbx_animedata.tex[state.tex_id]);
			shaderProgram.SetUniformValue(0, ref screenMatrix);
			
			//ブレンドファンクション
			switch( state.blend_func )
			{
			case 0:	//通常
				graphics.Enable( EnableMode.Blend, true );
				graphics.SetBlendFunc ( BlendFuncMode.Add, BlendFuncFactor.One, BlendFuncFactor.OneMinusSrcAlpha );
				break;
			case 1:	//乗算
				graphics.Enable( EnableMode.Blend, true );
				graphics.SetBlendFunc ( BlendFuncMode.Add, BlendFuncFactor.Zero, BlendFuncFactor.SrcColor );
				break;
			case 2:	//加算
				graphics.Enable( EnableMode.Blend, true );
				graphics.SetBlendFunc ( BlendFuncMode.Add, BlendFuncFactor.One, BlendFuncFactor.One );
				break;
			case 3:	//減算
				graphics.Enable( EnableMode.Blend, true );
				graphics.SetBlendFunc ( BlendFuncMode.Add, BlendFuncFactor.OneMinusSrcAlpha, BlendFuncFactor.OneMinusSrcColor );
				break;
			}
			graphics.DrawArrays(DrawMode.TriangleStrip, 0, indexSize);
		}
		private void get_rotation(ref float x, ref float y, float cx, float cy, float deg)
		{
			float dx = x - cx; // 中心からの距離(X)
			float dy = y - cy; // 中心からの距離(Y)
			float rad = FMath.Radians(deg);
			
			float tmpX = (dx * FMath.Cos(rad)) - (dy * FMath.Sin(rad)); // 回転
			float tmpY = (dx * FMath.Sin(rad)) + (dy * FMath.Cos(rad));
		
			x = (cx + tmpX); // 元の座標にオフセットする
			y = (cy + tmpY);
		
		}
	}
}
