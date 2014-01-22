import SpriteStudio
from struct import *



#------------------------------------------
#
#------------------------------------------
def main() : 
	ssxml = SpriteStudio.SSXML()
	print "sspj Load ..."
	
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
			anime_decoder.debug()
			#10フレーム目
			anime_decoder.setFrame(10)
			anime_decoder.update()			
			anime_decoder.debug()

			loopnum = anime_decoder.getPartNum()

			for i in range(loopnum):
				part = anime_decoder.getPart(i)
				#part.debug()
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
				print "part:inheritRates(0)=%s" % part.inheritRates(0)

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


