#ifndef __SSPLAYER_PARTSTATE__
#define __SSPLAYER_PARTSTATE__

//#include "../loader/ssloader.h"
//#include "../Helper/ssHelper.h"


class SsAnimeDecoder;
//class SsEffectRenderer;
class SsEffectRenderV2;



///パーツの状態を保持するクラスです。
struct SsPartState
{
	int				index;				///パーツのインデックスと一対一になるID

	float			vertices[3 * 5];	///< 座標
	float			colors[4 * 4];		///< カラー (４頂点分）
	float			uvs[2 * 5];			///< UV		(４隅＋中央)
	float			matrix[4 * 4];		///< 行列


	SsPartState*	parent;			/// 親へのポインタ
	float* 			inheritRates;	///< 継承設定の参照先。inheritType がparentだと親のを見に行く。透過的に遡るのでルートから先に設定されている必要がある。

	SsVector3		position;		///< 位置。あくまで親パーツ基準のローカル座標
	SsVector3		rotation;		///< 回転角。degree
	SsVector2		scale;			///< スケール	
	float			alpha;			///< 不透明度 0~1
	int				prio;			///< 優先度
	bool			hFlip;			///< 水平反転
	bool			vFlip;			///< 垂直反転
	bool			hide;			///< 非表示にする
	SsVector2		pivotOffset;	///< 原点のオフセット。旧SSの原点は左上基準にしたこの値に相当する。0,0が中央+0.5,+0.5が右上になる。参照セルがある場合はセルの原点に＋する＝オフセット扱いになる。
	SsVector2		anchor;			///< アンカーポイント。親パーツのどの位置に引っ付けるか？0,0が中央+0.5,+0.5が右上になる。
	SsVector2		size;			///< 表示サイズ	
	bool			imageFlipH;		///　セル画像を水平反転するか
 	bool			imageFlipV;		///	 セル画像を垂直反転するか
	SsVector2		uvTranslate;	///< UV 平行移動
	float			uvRotation;		///< UV 回転
	SsVector2		uvScale;		///< UV スケール
	float			boundingRadius;	///< 当たり判定用の円の半径

	SsCellValue		cellValue;		///< セルアニメの値
	SsColorAnime	colorValue;		///< カラーアニメの値
	SsVertexAnime	vertexValue;	///< 頂点アニメの値
	SsEffectAttr	effectValue;	///< エフェクトの値
	int				effectTime;
	float			effectTimeTotal;
	int				effectseed;

	bool			noCells;				/// セル参照が見つからない
	bool			is_color_blend;			/// カラーブレンドが使用される (描画コストが高いシェーダが使われるためフラグ化)
	bool			is_vertex_transform;	/// 頂点変形が使用される (描画コストが高いシェーダが使われるためフラグ化)

	SsInstanceAttr	instanceValue;

	SsBlendType::_enum	alphaBlendType;
		
	SsAnimeDecoder*		refAnime;
	SsEffectRenderV2*	refEffect;

	//V4互換計算用
	SsVector3		_temp_position;
	SsVector3		_temp_rotation;
	SsVector2		_temp_scale;


	SsPartState();

	virtual ~SsPartState();
	void	destroy();
	void	init();
	bool	inherits_(SsAttributeKind::_enum kind) const {return inheritRates[(int)kind] != 0.f;}
	void	reset();
};



#endif
