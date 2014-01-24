import SpriteStudio
from struct import *
from sstype import *
import os



def sstype_test() :
	print AttributeKindToSS4Tag(AttributeKind.prio)


#------------------------------------------
#
#------------------------------------------
def main() : 
	ssxml = SpriteStudio.SSXML()
	print "sspj Load ..."
	print  os.getcwd()
	print AttributeKindToSS4Tag( AttributeKind.prio )

	#車のデータを読んでみるよ
	if ssxml.Load("../TestData/car.sspj") == True :
		proj = ssxml.GetPrj() #ssxml.Loadで直接オブジェクトを得るで良いような
		proj.debug() #情報の取得

		#アニメパックを取得する
		#プロジェクトに格納されているアニメパックの数を得る
		print proj.getAnimePackNum()

		print "== Testing AnimePack =="
		#プロジェクトからアニメパックを取得（０番）
		animepack = proj.AnimePackAt(0)
		print animepack.debug()

		print "== Testing AnimeDecoder =="
		#アニメデコーダーの取得 animedecoderは使用済みになったら破棄される
		anime_decoder = animepack.getAnimeDecoderByName("motion 1")
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

				#試しにポジションＸをとってみる
				attribute = panime.getAttribute(AttributeKind.posx)

				if attribute == 0 :
					print "attibute is null."
				else:
					print "attibute is not null."
					if (key != 0 ) :
						key = attribute.firstKey()
						print "1st key time = %s" % key.time()

						if (key != 0 ) :
							key = attribute.nextKey()
							print "2nd key time = %s" % key.time()


		#セルマップの取得
		print "== Testing Cellmap =="
		print proj.getCellMapNum()


	#バイナリ形式で出力してみるよ
	f = open("test.dat", "wb")
	f.write(pack('B', 0x01))
	f.close();

#------------------------------------------
if  __name__ =="__main__":
	main()
	sstype_test()

