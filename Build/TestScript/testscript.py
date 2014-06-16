# -*- coding: utf-8 -*-
import SpriteStudio
import numpy
import os
from struct import *
from sstype import *



def test_printState(n,state) :
	print "state.vertices"
	print state.vertices
	print "state.colors"
	print state.colors
	print "state.uvs"
	print state.uvs
	print "state.matrix"
	print state.matrix
	print "[%s] state.position = (%s,%s,%s)" % ( n , state.position.x , state.position.y , state.position.z )
	print "[%s] state.rotation = (%s,%s,%s)" % ( n , state.rotation.x , state.rotation.y , state.rotation.z )
	print "[%s] state.scale = (%s,%s)" % ( n , state.scale.x , state.scale.y )
	print "[%s] state.alpha = (%s)" % ( n , state.alpha )
	print "[%s] state.prio = (%s)" % ( n , state.prio )
	print "[%s] state.hFlip = (%s)" % ( n , state.hFlip )
	print "[%s] state.vFlip = (%s)" % ( n , state.vFlip )
	print "[%s] state.hide = (%s)" % ( n , state.hide )
	print "[%s] state.pivotOffset = (%s,%s)" % ( n , state.pivotOffset.x , state.pivotOffset.y)
	print "[%s] state.anchor = (%s,%s)" % ( n , state.anchor.x , state.anchor.y)
	print "[%s] state.size = (%s,%s)" % ( n , state.size.x , state.size.y)
	print "[%s] state.imageFlipH = (%s)" % ( n , state.imageFlipH )
	print "[%s] state.imageFlipV = (%s)" % ( n , state.imageFlipV )
	print "[%s] state.uvTranslate = (%s,%s)" % ( n , state.uvTranslate.x , state.uvTranslate.y)
	print "[%s] state.uvRotation = (%s)" % ( n , state.uvRotation )
	print "[%s] state.uvScale = (%s,%s)" % ( n , state.uvScale.x , state.uvScale.y)
	print "[%s] state.boundingRadius = (%s)" % ( n , state.boundingRadius )
	print "[%s] state.noCells = (%s)" % ( n , state.noCells )
	print "[%s] state.is_color_blend = (%s)" % ( n , state.is_color_blend )
	print "[%s] state.is_vertex_transform = (%s)" % ( n , state.is_vertex_transform )
	print "[%s] state.alphaBlendType = (%s)" % ( n , state.alphaBlendType )

	#カラーブレンド値を取得
	if ( state.is_color_blend ) : #カラーブレンドを使用するフラグが立っている場合表示する
		color = state.colorValue.color
		print "[%s] state.colorValue.target = (%s)" % ( n , state.colorValue.target )
		print "[%s] state.colorValue.blendType = (%s)" % ( n , state.colorValue.blendType )
		print "[%s] state.colorValue.colors = (%s,%s,%s,%s)" % ( n , color.rgba.a ,color.rgba.r , color.rgba.g , color.rgba.b)
		color = state.colorValue.colors(0)
		print "[%s] state.colorValue.colors[0] = (%s,%s,%s,%s)" % ( n , color.rgba.a ,color.rgba.r , color.rgba.g , color.rgba.b)
		color = state.colorValue.colors(1)
		print "[%s] state.colorValue.colors[1] = (%s,%s,%s,%s)" % ( n , color.rgba.a ,color.rgba.r , color.rgba.g , color.rgba.b)
		color = state.colorValue.colors(2)
		print "[%s] state.colorValue.colors[2] = (%s,%s,%s,%s)" % ( n , color.rgba.a ,color.rgba.r , color.rgba.g , color.rgba.b)
		color = state.colorValue.colors(3)
		print "[%s] state.colorValue.colors[3] = (%s,%s,%s,%s)" % ( n , color.rgba.a ,color.rgba.r , color.rgba.g , color.rgba.b)

	if ( state.is_vertex_transform ) :
		vertex_anime = state.vertexValue
		print "vertex_transform"
		for loop in range(4) :							
			v = vertex_anime.offsets(loop)
			print "[%s] vertex = (%s,%s)" % ( loop , v.x ,v.y )

	#セル情報の取得
	#計算済みセルマップの取得 ただしuv移動などの適用前のデータになる。
	#適用後がほしい場合はpartStateのuvsを参照する
	#使用するテクスチャ、uv計算前の情報などが取得できる
	cellvalue = state.getCellValue()
	print "cellvalue.wrapMode = %s " % cellvalue.wrapMode
	print "cellvalue.filterMode = %s " % cellvalue.filterMode
	cell = cellvalue.getCell() #Bind_Cell取得
	if ( cell.isNull() == False):
		print "cell.name = <%s>" % cell.name
		print "cell.pos = (%s,%s)" % ( cell.pos.x , cell.pos.y )
		print "cell.size = (%s,%s)" % ( cell.size.x , cell.size.y )
		print "cell.pivot = (%s,%s)" % ( cell.pivot.x , cell.pivot.y )
		print "cell.rotated = %s" % cell.rotated

		texture = cellvalue.getTexture()
		print "texture.name %s" % texture.filename
		print "texture.width %s" % texture.width
		print "texture.height %s" % texture.height
		for loop in range(4) :							
			uvs = cellvalue.getUvs(loop)
			print "uvs[%s] = (%s,%s)" % ( loop , uvs.x ,uvs.y )

	#インスタンスパーツか否か？
	if ( state.isInstance ) :
		#update等は親で終わっているはずなのでそのまま出力
		print "[%s] state = instance?" % n
		inst_decoder = state.getInstanceDecoder()
		#inst_decoder.debug()
		loopnum = inst_decoder.getPartNum()
		for l in range(loopnum):
			istate = inst_decoder.getPartState(l)
			print "[%s] instance state.index = %s" % ( l , istate.index )
			test_printState( l , istate)


#===============================================================================
#bakeデータのテスト
#===============================================================================
def test_update() :
	print "== test_update =="
	ssxml = SpriteStudio.SSXML()

#	if ssxml.Load("../TestData/RoboProject.sspj") == True : #大仏のデータを読んでみるよ
	if ssxml.Load("../TestData/instance/stageanim3.sspj") == True : #大仏のデータを読んでみるよ
		proj = ssxml.GetPrj() 

		#プロジェクトからアニメパックを取得
		#animepack = proj.AnimePackAt(5)
		animepack = proj.AnimePackFromName("stageeffect2")

		#anime_decoder = animepack.getAnimeDecoderByName("robo_x")
		anime_decoder = animepack.getAnimeDecoderByName("BG")

		if anime_decoder != 0 :
			print "setting fps = %s" % anime_decoder.getAnimeFPS()
			print "setting framelength = %s" % anime_decoder.getFrameLength()

			#フレーム数計算する
			for i in range(anime_decoder.getFrameLength()):
#			for i in range(2):
				#計算するフレームを設定する 
				anime_decoder.setFrame(i)
				anime_decoder.update()			
				loopnum = anime_decoder.getPartNum()
				for n in range(loopnum):
					state = anime_decoder.getPartState(n)
					print "[%s] state.index = %s" % ( n , state.index )
					#表示してみる
					test_printState( n , state)

#					print "次元数 %s" % state.vertices.ndim #次元数
#					print "サイズ %s" % state.vertices.size #サイズ
#					print "各次元要素 %s" % state.vertices.shape #各次元要素
#					print "1要素のバイト数 %s" % state.vertices.itemsize #1要素のバイト数
#					print "行バイト %s" % state.vertices.strides #行バイト
#					print "配列全体のバイト %s" % state.vertices.nbytes #配列全体のバイト
#					print "配列のタイプ %s" % state.vertices.dtype #配列のタイプ
					#計算済みデータの出力テスト

#===============================================================================
#タグ変換テスト
#===============================================================================
def sstype_test() :
	print AttributeKindToSS4Tag(AttributeKind.prio)

#===============================================================================
#xml読み出しテスト
#===============================================================================
def test_ssxml() :
	ssxml = SpriteStudio.SSXML()
	print "sspj Load ..."
	print  os.getcwd()
	print AttributeKindToSS4Tag( AttributeKind.prio )

	
	#if ssxml.Load("../TestData/car.sspj") == True : #車のデータを読んでみるよ
	if ssxml.Load("../TestData/RoboProject.sspj") == True : #大仏のデータを読んでみるよ
		proj = ssxml.GetPrj() #ssxml.Loadで直接オブジェクトを得るで良いような
		proj.debug() #情報の取得

		#プロジェクト設定の読み出し
		print "== Testing Project Setting =="
		setting = proj.getSettings()
		print "setting.animeBaseDirectory = %s" % setting.animeBaseDirectory
		print "setting.cellMapBaseDirectory = %s" % setting.cellMapBaseDirectory
		print "setting.imageBaseDirectory = %s" % setting.imageBaseDirectory
		print "setting.exportBaseDirectory = %s" % setting.exportBaseDirectory
		print "setting.queryExportBaseDirectory = %s" % setting.queryExportBaseDirectory
		print "setting.wrapMode = %s" % setting.wrapMode
		print "setting.filterMode = %s" % setting.filterMode

		#アニメパックを取得する
		#プロジェクトに格納されているアニメパックの数を得る
		print proj.getAnimePackNum()

		print "== Testing AnimePack =="
		#プロジェクトからアニメパックを取得
		animepack = proj.AnimePackAt(5)
		print animepack.debug()

		print "== Testing AnimeDecoder =="
		#アニメデコーダーの取得 animedecoderは使用済みになったら破棄される
		#anime_decoder = animepack.getAnimeDecoderByName("motion 1")
		anime_decoder = animepack.getAnimeDecoderByName("robo_x")

		if anime_decoder != 0 :			
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

						#他に
						#findRightKey(time) timeより右のキーを取る
						#findLeftKey(time) timeより左のキーを取るがあります

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
						print "udat.rect=(%s,%s,%s,%s)" % ( udat.rect.x , udat.rect.y , udat.rect.w , udat.rect.h )
						print "udat.string=<%s>" % udat.string
						#次のキーを取得
						key = attribute.nextKey()


		#セルマップの取得
		testcell_name = ""
		print "== Testing Cellmap =="
		print proj.getCellMapNum()
		loopnum = proj.getCellMapNum()
		for i in range(loopnum):
			cellmap = proj.getCellMapAt(i)
			if ( i == 0 ) :
				testcell_name = cellmap.name

			print "cellmap.name = <%s> " % cellmap.name
			print "cellmap.imagePath = <%s> " % cellmap.imagePath
			print "cellmap.pixelSize = ( %s , %s )" % ( cellmap.pixelSize.x , cellmap.pixelSize.y )
			print "cellmap.overrideTexSettings = %s " % cellmap.overrideTexSettings
			print "cellmap.wrapMode = %s " % cellmap.wrapMode
			print "cellmap.filterMode = %s " % cellmap.filterMode
			print "cellmap.length = %s " % cellmap.length

			for n in range(cellmap.length):
				cell = cellmap.getCell(n)				
				print "cell.name = <%s>" % cell.name
				print "cell.pos = (%s,%s)" % ( cell.pos.x , cell.pos.y )
				print "cell.size = (%s,%s)" % ( cell.size.x , cell.size.y )
				print "cell.pivot = (%s,%s)" % ( cell.pivot.x , cell.pivot.y )
				print "cell.rotated = %s" % cell.rotated

		print "== Testing Cellmap(getCellMapFromName) =="
		cellmap = proj.getCellMapFromName(testcell_name)
		print "cellmap.name = <%s> " % cellmap.name



#------------------------------------------
#
#------------------------------------------
def main() : 

#	test_ssxml()
	test_update()
	

#------------------------------------------
if  __name__ =="__main__":
	main()
	sstype_test()

