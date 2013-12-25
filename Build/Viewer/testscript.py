import SpriteStudio
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
		#アニメデコーダーの取得 animedevoderは使用済みになったら破棄される
		anime_decoder = animepack.getAnimeDecoderByName("motion 1")
		if anime_decoder != 0 :
			anime_decoder.debug()

		#セルマップの取得
		print "== Testing Cellmap =="
		print proj.getCellMapNum()


	#バイナリ形式で出力してみるよ
	with open("test.dat", "wb") as fout:
		bary = bytearray([0xFF, 0x12, 0x89])
		bary.append(0)
		bary.extend([1, 127])
		fout.write(bary)

#------------------------------------------
if  __name__ =="__main__":
	main()


