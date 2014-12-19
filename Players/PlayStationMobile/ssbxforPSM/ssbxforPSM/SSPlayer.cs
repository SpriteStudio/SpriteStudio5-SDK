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
		// スプライト。
		protected struct Vertex
		{
			public Short2 Position; //4byte
			public Short2 UV;		//4byte
			public UByte4N Color;	//4byte
		}
		protected struct SpritePrim
		{
			public Vertex V0;
			public Vertex V1;
			public Vertex V2;
			public Vertex V3;
		}
	
		private SpritePrim[] spritePrimBuffer;
		private VertexBuffer[] vertexBuffer = new VertexBuffer[2];
		protected Matrix4 wvp;
		protected int[] primCount = new int[2]{0,0};
		protected int index = 0;
		protected ShaderProgram shaderProgram;
		const int maxNumOfSprite = 256;
		private int oldtexid = -1;
		private int vidx = 0;
		private int old_blend_func;
		
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
			shaderProgram.SetAttributeBinding(0,"a_Position");
			shaderProgram.SetAttributeBinding(1,"a_Color");
			shaderProgram.SetUniformBinding(0,"u_WVP");

			int width = graphics.GetFrameBuffer().Width;
			int height = graphics.GetFrameBuffer().Height;		
			Matrix4 proj = Matrix4.Ortho(
				0, width,
				0, height,
				0, 32768.0f
				);
			
			Matrix4 view = Matrix4.LookAt(
					new Vector3(0, height, 0),
					new Vector3(0, height, 1),
					new Vector3(0, -1, 0));
			wvp = proj * view;
			
			int i;
			// 頂点バッファを2つ作成。
			for(i = 0; i < vertexBuffer.Length; ++i){
				vertexBuffer[i] = new VertexBuffer(
					4*maxNumOfSprite,
					6*maxNumOfSprite,
					VertexFormat.Short4,
					VertexFormat.UByte4N
					);
			}
	
			// index
			{
				int numIndices = 6 * maxNumOfSprite;
				ushort[] indices = new ushort[numIndices];
				int wp;
				for(i = 0, wp = 0; i < maxNumOfSprite; ++i)
				{
					int vnum = i*4;
	
					indices[wp++] = (ushort)(0+vnum);
					indices[wp++] = (ushort)(1+vnum);
					indices[wp++] = (ushort)(2+vnum);
	
					indices[wp++] = (ushort)(1+vnum);
					indices[wp++] = (ushort)(3+vnum);
					indices[wp++] = (ushort)(2+vnum);
				}
				vertexBuffer[0].SetIndices(indices);
				vertexBuffer[1].SetIndices(indices);
	
			}
	
			spritePrimBuffer = new SpritePrim[maxNumOfSprite];
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
		//透明度を設定
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

			oldtexid = -1;
			vidx = 0;
			old_blend_func = -1;
			
			string str;
			str = string.Format("frame{0:D3}", nowframe);
			SSBX_FRAMEDATA ssbx_framedata = ssbx_motiondata.framedata[str];
			int idx = 0;
			for ( idx = 0; idx < ssbx_framedata.framedata.Count; idx++ )
			{
				str = string.Format("part{0:D3}", idx);
				SSBX_PARTDATA ssbx_partdata = ssbx_framedata.framedata[str];
				PART_STATE state = ssbx_partdata.partdata;
				
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
			if ( vidx > 0 )
			{
				graphics.SetTexture(0, ssbx_animedata.tex[oldtexid]);
				blt_render();
			}
		}
		
		private void sprite_draw( PART_STATE state )
		{
			if ( state.tex_id == -1 )
			{
				//NULLパーツ
				return;
			}
			
			if ((oldtexid != state.tex_id) || ( old_blend_func != state.blend_func ) )
			{
				if ( vidx > 0 )
				{
					graphics.SetTexture(0, ssbx_animedata.tex[oldtexid]);
					blt_render();
				}
				oldtexid = state.tex_id;
				old_blend_func = state.blend_func;
			}
			UByte4N _color = new UByte4N(1.0f,1.0f,1.0f,alpha);
			
			spritePrimBuffer[vidx].V0.Color = _color;
			spritePrimBuffer[vidx].V1.Color = _color;
			spritePrimBuffer[vidx].V2.Color = _color;
			spritePrimBuffer[vidx].V3.Color = _color;
			
			float tex_w = ssbx_animedata.tex[state.tex_id].Width;
			float tex_h = ssbx_animedata.tex[state.tex_id].Height;
			float u0 = (float)state.rect_x / tex_w;
			float v0 = (float)state.rect_y / tex_h;
			float u1 = (float)( state.rect_x + state.rect_w ) / tex_w;
			float v1 = (float)( state.rect_y + state.rect_h ) / tex_h;

			Short2[] _uv = new Short2[4];
			_uv[0] = new Short2((short)(u0*32767),(short)(v0*32767));//(0,0)
			_uv[1] = new Short2((short)(u0*32767),(short)(v1*32767));//(1,0)
			_uv[2] = new Short2((short)(u1*32767),(short)(v0*32767));//(0,1)
			_uv[3] = new Short2((short)(u1*32767),(short)(v1*32767));//(1,1)
			
			spritePrimBuffer[vidx].V0.UV = _uv[0];
			spritePrimBuffer[vidx].V1.UV = _uv[1];
			spritePrimBuffer[vidx].V2.UV = _uv[2];
			spritePrimBuffer[vidx].V3.UV = _uv[3];
		
			short w = (short)( state.rect_w * state.scale_x * scale_x );
			short h = (short)( state.rect_h * state.scale_y * scale_y );
			short x = (short)( state.x - ( w / 2 ) );
			short y = (short)( state.y - ( h / 2 ) );
			
			spritePrimBuffer[vidx].V0.Position.X = x;//0.0f;	// x0
			spritePrimBuffer[vidx].V0.Position.Y = y;//0.0f;	// y0
			
			spritePrimBuffer[vidx].V1.Position.X = x;//0.0f;	// x1
			spritePrimBuffer[vidx].V1.Position.Y = (short)(y + h);//1.0f;	// y1
	
			spritePrimBuffer[vidx].V2.Position.X = (short)(x + w);//1.0f;	// x2
			spritePrimBuffer[vidx].V2.Position.Y = y;//0.0f;	// y2
			
			spritePrimBuffer[vidx].V3.Position.X = (short)(x + w);//1.0f;	// x3
			spritePrimBuffer[vidx].V3.Position.Y = (short)(y + h);//1.0f;	// y3

			if(state.rotz != 0.0f)
			{
				float rx = 0;
				float ry = 0;
				

				float fw = ( state.rect_w * state.scale_x * scale_x );
				float fh = ( state.rect_h * state.scale_y * scale_y );
				float fx = (float)state.x - ( w / 2.0f );
				float fy = (float)state.y - ( h / 2.0f );
				
				rx = fx;
				ry = fy;
				get_rotation(ref rx, ref ry, (float)state.x, (float)state.y, state.rotz);
				spritePrimBuffer[vidx].V0.Position.X = (short)rx;
				spritePrimBuffer[vidx].V0.Position.Y = (short)ry;

				rx = fx;
				ry = fy + fh;
				get_rotation(ref rx, ref ry, (float)state.x, (float)state.y, state.rotz);
				spritePrimBuffer[vidx].V1.Position.X = (short)rx;
				spritePrimBuffer[vidx].V1.Position.Y = (short)ry;

				rx = fx + fw;
				ry = fy;
				get_rotation(ref rx, ref ry, (float)state.x, (float)state.y, state.rotz);
				spritePrimBuffer[vidx].V2.Position.X = (short)rx;
				spritePrimBuffer[vidx].V2.Position.Y = (short)ry;

				rx = fx + fw;
				ry = fy + fh;
				get_rotation(ref rx, ref ry, (float)state.x, (float)state.y, state.rotz);
				spritePrimBuffer[vidx].V3.Position.X = (short)rx;
				spritePrimBuffer[vidx].V3.Position.Y = (short)ry;
			}
			
			vidx++;
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
		protected void blt_render()
		{
			//ブレンドファンクション
			switch( old_blend_func )
			{
			case 0:	//通常
				graphics.Enable( EnableMode.Blend, true );
				graphics.SetBlendFunc ( BlendFuncMode.Add, BlendFuncFactor.SrcAlpha, BlendFuncFactor.OneMinusSrcAlpha );
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
			
			primCount[index] = vidx;
			// 必要な分だけ転送。
			vertexBuffer[index].SetVertices(spritePrimBuffer,0,0,primCount[index]*4);
			shaderProgram.SetUniformValue(0,ref wvp);
				
			graphics.SetShaderProgram(shaderProgram);
			
			graphics.SetVertexBuffer(0,vertexBuffer[index]);
			graphics.DrawArrays(
				DrawMode.Triangles,
				0,
				primCount[index]*6
				);
			
			oldtexid = -1;
			vidx = 0;
		}
	}
}
