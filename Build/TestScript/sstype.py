class AttributeKind:
	invalid=-1
	cell=0
	posx=1
	posy=2
	posz=3
	rotx=4
	roty=5
	rotz=6
	sclx=7
	scly=8
	alpha=9
	prio=10
	fliph=11
	flipv=12
	hide=13
	color=14
	vertex=15
	pivotx=16
	pivoty=17
	anchorx=18
	anchory=19
	sizex=20
	sizey=21
	imgfliph=22
	imgflipv=23
	uvtx=24
	uvty=25
	uvrz=26
	uvsx=27
	uvsy=28
	boundr=29
	user=30
	num = 31

AttributeKindDic = {
	"invalid" : -1,
	"cell" : 0,
	"posx" : 1,
	"posy" : 2,
	"posz" : 3,
	"rotx" : 4,
	"roty" : 5,
	"rotz" : 6,
	"sclx" : 7,
	"scly" : 8,
	"alpha" : 9,
	"prio"  : 10, 
	"fliph" : 11,
	"flipv" : 12,
	"hide"  : 13,
	"color" : 14,
	"vertex": 15,
	"pivotx": 16,
	"pivoty": 17,
	"anchorx":18,
	"anchory":19,
	"sizex":  20,
	"sizey":  21,
	"imgfliph":22,
	"imgflipv":23,
	"uvtx":24,
	"uvty":25,
	"uvrz":26,
	"uvsx":27,
	"uvsy":28,
	"boundr":29,
	"user":30,
}

AttributeKindSS4Dic = {
	-1 	: "invalid",
	0  	: "CELL",
	1  	: "POSX",
	2 	: "POSY" ,
	3 	: "-" ,
	4 	: "-" ,
	5 	: "-" ,
	6 	: "ANGL" ,
	7 	: "SCAX" ,
	8 	: "SCAX" ,
	9 	: "TRAN" ,
	10 	: "PRIO" , 
	11 	: "FLPH" ,
	12 	: "FLPV" ,
	13 	: "HIDE"  ,
	14 	: "PCOL" ,
	15 	: "VERT",
	16 	: "ORFX",
	17 	: "ORFY",
	18 	: "-",
	19 	: "-",
	20 	: "-",
	21 	: "-",
	22 	: "-",
	23 	: "-",
	24 	: "IMGX",
	25 	: "IMGY",
	26 	: "-",
	27 	: "IMGW",
	28 	: "IMGH",
	29 	: "-",
	30 	: "UDAT",
}

def AttributeKindToSS4Tag(kind):
	return AttributeKindSS4Dic[kind]
