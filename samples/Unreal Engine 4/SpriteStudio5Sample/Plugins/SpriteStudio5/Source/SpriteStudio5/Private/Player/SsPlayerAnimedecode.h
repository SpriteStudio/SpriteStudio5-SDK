#pragma once

class FSsAnimeDecoder;
class USsProject;
class FSsCelMapLinker;
class FSsCellMapList;
struct FSsKeyframe;
struct FSsAttribute;
struct FSsPart;
struct FSsPartAnime;
struct FSsPartState;
struct FSsCellValue;
struct FSsAnimation;


//パーツとアニメを関連付ける
typedef TPair<FSsPart*, FSsPartAnime*> FSsPartAndAnime;


class FSsAnimeDecoder
{
public:
	struct DrawOption
	{
		// 変換
		FVector2D CenterLocation;
		float Rotation;
		FVector2D Scale;

		bool bFlipH;	// 左右反転
		bool bFlipV;	// 上下反転
		TMap<int32, TWeakObjectPtr<UTexture>>* TextureReplacements;	// パーツ毎のテクスチャ差し替え

		bool bDrawMask;	// 問題対処用の臨時メンバ. 詳細はSsPlayerComponent.hのRenderTargetMask宣言辺りのMEMOにて. 

		DrawOption()
			: CenterLocation(0.f, 0.f)
			, Rotation(0.f)
			, Scale(1.f, 1.f)
			, bFlipH(false)
			, bFlipV(false)
			, TextureReplacements(NULL)
		{}
	};

private:

	///パーツ情報とパーツアニメーションを結びつけアレイにしたもの
	TArray<FSsPartAndAnime>	PartAnime;

	///パーツ名からアニメ情報をとるために使うもし、そういった用途が無い場合はローカル変数でも機能する
	TMap<FName,FSsPartAnime*> PartAnimeDic;
	
	FSsCellMapList*		CurCellMapManager;		///アニメに関連付けられているセルマップ

	FSsPartState*			PartState;			///パーツの現在の状態が格納されています。
	TArray<FSsPartState*>	SortList;			///ソート状態

	float			NowPlatTime;
	float			FrameDelta;
	int				CurAnimeEndFrame;
	int				CurAnimeFPS;
	FSsAnimation*	CurAnimation;

private:
	void	UpdateState( int nowTime , FSsPart* part , FSsPartAnime* part_anime , FSsPartState* state );
	void	UpdateInstance( int nowTime , FSsPart* part , FSsPartAnime* part_anime , FSsPartState* state );
	void	UpdateMatrix(FSsPart* part , FSsPartAnime* anime , FSsPartState* state);
	void	UpdateVertices(FSsPart* part , FSsPartAnime* anime , FSsPartState* state);

	int		CalcAnimeLabel2Frame(const FName& str, int offset, FSsAnimation* Animation);
	int		FindAnimetionLabel(const FName& str, FSsAnimation* Animation);

	void	DrawPart(FCanvas* Canvas, FSsPartState* State, const DrawOption& Option);

public:
	FSsAnimeDecoder();
	virtual ~FSsAnimeDecoder();

	virtual void	Update();
	virtual void	Draw(FCanvas* Canvas, const DrawOption& Option=DrawOption());

	void SetDeltaForIndependentInstance(float Delta) { FrameDelta = Delta * CurAnimeFPS; }

	void	SetAnimation(struct FSsModel* model, FSsAnimation* anime, FSsCellMapList* cellmap, USsProject* sspj=0);
	bool	IsAnimationValid() const { return (NULL != CurAnimation); }

	void	SetPlayFrame(float time) { NowPlatTime = time; FrameDelta = 0.f; }
	float	GetPlayFrame() const { return NowPlatTime; }
	int		GetAnimeEndFrame() { return CurAnimeEndFrame; }
	int		GetAnimeFPS() { return CurAnimeFPS; }

	// パーツ名からインデックスを取得
	int		GetPartIndexFromName(FName PartName) const;

	// パーツのTransformを取得(Canvas内2D座標系)
	FTransform GetPartTransformInCanvas(int PartIndex, int32 CanvasWidth, int32 CanvasHeight, const FSsAnimeDecoder::DrawOption& Option) const;


	TArray<FSsPartState*>& GetPartSortList(){return SortList;}
	const TArray<FSsPartAndAnime>& GetPartAnime(){return PartAnime;}
	
	template<typename mytype> int	SsGetKeyValue(int time , FSsAttribute* attr , mytype&  value);
	template<typename mytype> void	SsInterpolationValue(int time , const FSsKeyframe* leftkey , const FSsKeyframe* rightkey , mytype& v);
	void	SsInterpolationValue(int time , const FSsKeyframe* leftkey , const FSsKeyframe* rightkey , bool& v);
	void	SsInterpolationValue(int time , const FSsKeyframe* leftkey , const FSsKeyframe* rightkey , FSsCellValue& v);
	void	SsInterpolationValue(int time , const FSsKeyframe* leftkey , const FSsKeyframe* rightkey , FSsColorAnime& v);
	void	SsInterpolationValue(int time , const FSsKeyframe* leftkey , const FSsKeyframe* rightkey , FSsVertexAnime& v);
	void	SsInterpolationValue(int time , const FSsKeyframe* leftkey , const FSsKeyframe* rightkey , FSsInstanceAttr& v);
};

