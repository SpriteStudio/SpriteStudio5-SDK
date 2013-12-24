import SpriteStudio
#------------------------------------------
#
#------------------------------------------
def main() : 
	ssio = SpriteStudio.SSXML()
	print "sspj Load ..."
	
	#車のデータを読んでみるよ
	if ssio.Load("../TestData/car.sspj") == True :
		proj = ssio.GetPrj() #ssio.Loadで直接オブジェクトを得るで良いような
		proj.debug() #情報の取得

		#プロジェクトに格納されているアニメパックの数を得る
		print proj.getAnimePackNum()

		#アニメパックを取得する
		print "== Testing AnimePack =="
		animepack = proj.AnimePackAt(0)
		print animepack.debug()

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


