import SpriteStudio
from struct import *
from sstype import *
import os



def sstype_test() :
	print AttributeKindToSS4Tag(AttributeKind.prio)


def test_ssxml() :
	ssxml = SpriteStudio.SSXML()
	print "sspj Load ..."
	print  os.getcwd()
	print AttributeKindToSS4Tag( AttributeKind.prio )

	
	#if ssxml.Load("../TestData/car.sspj") == True : #車のデータを読んでみるよ
	if ssxml.Load("../TestData/RoboProject.sspj") == True : #大仏のデータを読んでみるよ
		proj = ssxml.GetPrj() #ssxml.Loadで直接オブジェクトを得るで良いような
		proj.debug() #情報の取得

		#アニメパックを取得する
		#プロジェクトに格納されているアニメパックの数を得る
		print proj.getAnimePackNum()

		print "== Testing AnimePack =="
		#プロジェクトからアニメパックを取得（０番）
		animepack = proj.AnimePackAt(5)
		print animepack.debug()

		print "== Testing AnimeDecoder =="
		#アニメデコーダーの取得 animedecoderは使用済みになったら破棄される
		#anime_decoder = animepack.getAnimeDecoderByName("motion 1")
		anime_decoder = animepack.getAnimeDecoderByName("robo_x")

		if anime_decoder != 0 :
			# 1フレーム毎の計算済みデータの取得
			#0フレーム目
			anime_decoder.setFrame(0)
			anime_decoder.update()			
			anime_decoder.debug() #デバッグ表示
			#10フレーム目
			anime_decoder.setFrame(10)
			anime_decoder.update()			
			anime_decoder.debug() #デバッグ表示
			
			#パーツを取得してみる これは計算されておらず純粋にＸＭＬドキュメントを統合して整理されたり
			#要素同士が結びつけられたデータとなる
			loopnum = anime_decoder.getPartNum()

			#正直普通にXMLを解析するという手もあるが利点としては既にオブジェクト同士のリンクがとられている所だろう			
			#Testing Part & partAnime
			for i in range(loopnum):
				part = anime_decoder.getPart(i)
				#part.debug()
				#パーツ情報を取得
				print "== part Method Test =="
				print "part:name=%s" % part.name()
				print "part:arrayIndex=%s" % part.arrayIndex()
				print "part:parentIndex=%s" % part.parentIndex()
				print "part:type=%s" % part.type()
				print "part:boundsType=%s" % part.boundsType()
				print "part:inheritType=%s" % part.inheritType()
				print "part:alphaBlendType=%s" % part.alphaBlendType()
				print "part:show=%s" % part.show()
				print "part:locked=%s" % part.locked()
				print "part:inheritRates(alpha)=%s" % part.inheritRates(AttributeKind.alpha)
				print "part:inheritRates(fliph)=%s" % part.inheritRates(AttributeKind.fliph)
				print "part:inheritRates(flipv)=%s" % part.inheritRates(AttributeKind.flipv)
				print "part:inheritRates(hide)=%s" % part.inheritRates(AttributeKind.hide)
				# panime.name と part.nameは一致する
				panime = anime_decoder.getPartAnime(i) 
				print "panime:name=%s" % panime.name()
				
				#パーツ情報の取得サンプル
				#試しにposition.xをとってみる
				attribute = panime.getAttribute(AttributeKind.posx)
				
				if attribute.isNull() :
					print "attibute is null."
				else:
					key = attribute.firstKey()
					while key.isNull() == False:
						print "key time = %s %s" % ( key.time() , InterpolationTypeToString(key.interpolationType()) )
						#カーブパラメータの取得
						#カーブの値はプロパティ扱いになっているので注意
						curve = key.getCurveParam()
						print "curve.start = %s " % curve.startTime
						print "curve.startValue = %s " % curve.startValue
						print "curve.endTime = %s " % curve.endTime
						print "curve.endValue = %s " % curve.endValue
						print "curve.startKeyTime = %s " % curve.startKeyTime
						print "curve.endKeyTime = %s " % curve.endKeyTime
						
						#頂点カラーや頂点変形、セルマップ、ユーザーデータ以外のデータはこのように取得する
						v = key.getValue()
						print "value type=%s v(int)=%s" % ( v.getType() , v.toInt() ) #intでとってみる
						print "value type=%s v(float)=%s" % ( v.getType() , v.toFloat() ) #floatでとってみる
						print "value type=%s v(bool)=%s" % ( v.getType() , v.toBool() ) #boolでとってみる
						
						#次のキーを取得
						key = attribute.nextKey()

				#頂点カラー値をとる
				attribute = panime.getAttribute(AttributeKind.color)
				if attribute.isNull() :
					print "color attibute is null."
				else:
					key = attribute.firstKey()
					while key.isNull() == False:
						color_anime = key.getColorAnime() # ColorAnime型へ変換する
						print "color_anime target=%s blendType=%s" % ( color_anime.target , color_anime.blendType )
						#全体にかける場合のカラー値を取得する
						print "color_anime color=%x rate=%s" % ( color_anime.color.rgba.toARGB() , color_anime.color.rate) 
						#頂点ごとのカラー値を取得する
						print "color_anime colors(0)=%x rate= %s" % ( color_anime.colors(0).rgba.toARGB() , color_anime.colors(0).rate )
						print "color_anime colors(1)=%x rate= %s" % ( color_anime.colors(1).rgba.toARGB() , color_anime.colors(1).rate )
						print "color_anime colors(2)=%x rate= %s" % ( color_anime.colors(2).rgba.toARGB() , color_anime.colors(2).rate )
						print "color_anime colors(3)=%x rate= %s" % ( color_anime.colors(3).rgba.toARGB() , color_anime.colors(3).rate )

						#次のキーを取得
						key = attribute.nextKey()

				#頂点変形値を取る
				attribute = panime.getAttribute(AttributeKind.vertex)
				if attribute.isNull() :
					print "vertex attibute is null."
				else:
					key = attribute.firstKey()
					while key.isNull() == False:
						vertex_anime = key.getVertexAnime() # ColorAnime型へ変換する
						
						print "vertex_anime(0) x=%s y=%s" % ( vertex_anime.offsets(0).x ,vertex_anime.offsets(0).y )
						print "vertex_anime(1) x=%s y=%s" % ( vertex_anime.offsets(1).x ,vertex_anime.offsets(1).y )
						print "vertex_anime(2) x=%s y=%s" % ( vertex_anime.offsets(2).x ,vertex_anime.offsets(2).y )
						print "vertex_anime(3) x=%s y=%s" % ( vertex_anime.offsets(3).x ,vertex_anime.offsets(3).y )
						
						#次のキーを取得
						key = attribute.nextKey()
						

				attribute = panime.getAttribute(AttributeKind.cell)
				if attribute.isNull() :
					print "cell attibute is null."
				else:
					key = attribute.firstKey()
					while key.isNull() == False:
						#参照セル値を取得する
						cell = key.getRefCell()

						print "cell.mapid=<%s>" % cell.mapid 
						print "cell.name=<%s>" % cell.name 

						#次のキーを取得
						key = attribute.nextKey()


				attribute = panime.getAttribute(AttributeKind.user)
				if attribute.isNull() :
					print "udat attibute is null."
				else:
					key = attribute.firstKey()
					while key.isNull() == False:
						#参照セル値を取得する
						udat = key.getUserDataAnime()

						print "udat.useInteger=<%s>" % udat.useInteger						
						print "udat.usePoint=<%s>" % udat.usePoint
						print "udat.useRect=<%s>" % udat.useRect
						print "udat.useString=<%s>" % udat.useString


						print "udat.integer=<%s>" % udat.integer					
						print "udat.point=(%s,%s)" % ( udat.point.x , udat.point.y )
						print "udat.point=(%s,%s,%s,%s)" % ( udat.rect.x , udat.rect.y , udat.rect.w , udat.rect.h )
						print "udat.string=<%s>" % udat.string


						#次のキーを取得
						key = attribute.nextKey()



		#セルマップの取得
		print "== Testing Cellmap =="
		print proj.getCellMapNum()



#------------------------------------------
#
#------------------------------------------
def main() : 

	test_ssxml()

	#バイナリ形式で出力してみるよ	（途中
	f = open("test.dat", "wb")
	f.write(pack('B', 0x01))
	f.close();

#------------------------------------------
if  __name__ =="__main__":
	main()
	sstype_test()

