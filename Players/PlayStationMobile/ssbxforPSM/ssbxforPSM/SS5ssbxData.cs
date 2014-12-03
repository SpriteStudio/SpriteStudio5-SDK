
using System;
using System.Collections.Generic;
using System.Xml.Linq;
using Sce.PlayStation.Core.Graphics;

namespace ss
{
	//パーツステータス
	public struct PART_STATE
	{
		public string name;
		public float x;
		public float y;
		public float rotz;
		public float scale_x;
		public float scale_y;
		public float alpha;
		public float size_x;
		public float size_y;
		public float boundingRadius;
		public int flip_x;
		public int flip_y;
		public int tex_id;
		public int rect_x;
		public int rect_y;
		public int rect_w;
		public int rect_h;
		public int blend_func;
	}
	//パーツデータクラス
	public class SSBX_PARTDATA
	{
		public PART_STATE partdata;
		public SSBX_PARTDATA(PART_STATE data)
		{
			this.partdata = data;
		}
	}
	
	//フレームデータクラス
	public class SSBX_FRAMEDATA
	{
		public Dictionary<string, SSBX_PARTDATA> framedata;
		public SSBX_FRAMEDATA(Dictionary<string, SSBX_PARTDATA> framedata)
		{
			this.framedata = framedata;
		}
	}

	//テクスチャデータクラス
	public class SSBX_TEXTUREDATA
	{
		public int id;
		public string name;
		public SSBX_TEXTUREDATA(int id, string name)
		{
			this.id = id;
			this.name = name;
		}
	}
	
	
	//モーションデータクラス
	public class SSBX_MOTIONDATA
	{
		public string motionname;
		public int fps;
		public int maxframe;
		public Dictionary<string, SSBX_FRAMEDATA> framedata;
		public SSBX_MOTIONDATA(string motionname, int fps, int maxframe, Dictionary<string, SSBX_FRAMEDATA> framedata )
		{
			this.motionname = motionname;
			this.fps = fps;
			this.maxframe = maxframe;
			this.framedata = framedata;
		}
	}

	//アニメデータクラス
	public class SSBX_ANIMEDATA
	{
		public string sspbname;
		public Dictionary<string, SSBX_TEXTUREDATA> texturedata;
		public Dictionary<string, SSBX_MOTIONDATA> motiondata;
		//テクスチャ
		public Texture2D[] tex = new Texture2D[32];
		
		public SSBX_ANIMEDATA(string sspbname, Dictionary<string, SSBX_TEXTUREDATA> texturedata, Dictionary<string, SSBX_MOTIONDATA> motiondata )
		{
			this.sspbname = sspbname;
			this.texturedata = texturedata;
			this.motiondata = motiondata;

			//モーションデータからテクスチャ情報を取得して画像読み込み
			int idx;
			for ( idx = 0; idx < texturedata.Count; idx++ )
			{
				string str;
				str = string.Format("texture{0:D3}", idx);
				SSBX_TEXTUREDATA ssbx_texturedata = texturedata[str];
				int id = ssbx_texturedata.id;
				string name = "/Application/resources/" + ssbx_texturedata.name;

				//画像読み込み
				tex[idx] = new Texture2D(name, false, PixelFormat.Rgba);
			}
		}
	}
	
	//SSBXからアニメデータを解析して、連想配列を取得します。
	public class SSBX
	{
		static public SSBX_ANIMEDATA GetSsbx_animedata(string xmlFilename)
		{
			SSBX_ANIMEDATA animedata = null;
			string str;
			
			try
			{
				XDocument doc = XDocument.Load(xmlFilename);
				
				//------------------------------------------
				//ssbxはアニメの設定などの情報、使用するテクスチャの情報、表示するパーツのフレーム情報から構成されています。
				//------------------------------------------

				//ssbpdataエレメントの取得
				XElement ssbx = doc.Element("ssbx");
				string sspbname = ssbx.Element("name").Value;	//ssbp名

				XElement ssbpdata = ssbx.Element("ssbpdata");
				//テクスチャーデータの取得
				Dictionary<string, SSBX_TEXTUREDATA> dic_texturedata = new Dictionary<string, SSBX_TEXTUREDATA>();
				{
					XElement textures = ssbx.Element("textures");
					
					int idx = 0;
					foreach( var texture in textures.Descendants("texture"))
					{
						int id = int.Parse(texture.Attribute("id").Value);
						string name = texture.Attribute("name").Value;
	
						str = string.Format("texture{0:D3}", idx);
						dic_texturedata.Add(str,new SSBX_TEXTUREDATA(id, name));
						idx++;
					}
				}

				Dictionary<string, SSBX_MOTIONDATA> dic_motiondata = new Dictionary<string, SSBX_MOTIONDATA>();
				foreach( var motion in ssbx.Descendants("motion"))
				{

					//アニメ設定の取得
					string motionname = motion.Element("name").Value;
					int fps = int.Parse(motion.Element("fps").Value);
					int maxframe = int.Parse(motion.Element("maxframe").Value);
				
	
					//フレームデータの取得
					Dictionary<string, SSBX_FRAMEDATA> dic_ssbx_framedata = new Dictionary<string, SSBX_FRAMEDATA>();
					{
						XElement framedata = motion.Element("framedata");
						foreach( var frame in framedata.Descendants("frame"))
						{
							//各フレームに含まれるパーツ情報を格納していく、
							//フレームデータ連想配列　に　パーツデータ連想配列を追加していく。
							int time = int.Parse(frame.Attribute("time").Value);
	//						Console.WriteLine("time" + time);
							Dictionary<string, SSBX_PARTDATA> dic_ssbx_partdata = new Dictionary<string, SSBX_PARTDATA>();
							
							int idx = 0;
							foreach( var partdata in frame.Descendants("partdata"))
							{
		
								PART_STATE part;
								part.name = partdata.Attribute("NAME").Value;
								part.x = float.Parse(partdata.Attribute("X").Value);
								part.y = float.Parse(partdata.Attribute("Y").Value);
								part.rotz = float.Parse(partdata.Attribute("ROTZ").Value);
								part.scale_x = float.Parse(partdata.Attribute("SCLX").Value);
								part.scale_y = float.Parse(partdata.Attribute("SCLY").Value);
								part.alpha = float.Parse(partdata.Attribute("ALPH").Value) / 255.0f;
								part.size_x = float.Parse(partdata.Attribute("SIZX").Value);
								part.size_y = float.Parse(partdata.Attribute("SIZY").Value);
								part.boundingRadius = float.Parse(partdata.Attribute("BORA").Value);
								part.flip_x = int.Parse(partdata.Attribute("FLPX").Value);
								part.flip_y = int.Parse(partdata.Attribute("FLPY").Value);
								part.tex_id = int.Parse(partdata.Attribute("TXID").Value);
								part.rect_x = int.Parse(partdata.Attribute("RECX").Value);
								part.rect_y = int.Parse(partdata.Attribute("RECY").Value);
								part.rect_w = int.Parse(partdata.Attribute("RECW").Value);
								part.rect_h = int.Parse(partdata.Attribute("RECH").Value);
								part.blend_func = int.Parse(partdata.Attribute("BLED").Value);
		//						Console.WriteLine("partdata x" + x + " y" + y + " ROTZ" + rotz + " SCLX" + scale_x + " SCLY" + scale_y + " ALPH" + alpha + " SIZX" + size_x + " SIZY" + size_y );
								
								//パーツデータ配列のアクセス名を設定
								str = string.Format("part{0:D3}", idx);
								dic_ssbx_partdata.Add(str,new SSBX_PARTDATA(part));
								idx++;
							}
							//フレームデータ配列のアクセス名を設定
							str = string.Format("frame{0:D3}", time);
							dic_ssbx_framedata.Add(str,new SSBX_FRAMEDATA(dic_ssbx_partdata));
							
						}
					}
					dic_motiondata.Add(motionname, new SSBX_MOTIONDATA(motionname, fps, maxframe, dic_ssbx_framedata));
				}
				animedata = new SSBX_ANIMEDATA(sspbname,dic_texturedata,dic_motiondata);
			}
			catch (Exception e)
			{
			    Console.Error.WriteLine(e.Message);
			    Environment.Exit(1);
			}
			
			return animedata;
		}
	}

}

