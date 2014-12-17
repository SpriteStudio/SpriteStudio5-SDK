#pragma once

#include "SsPlayerTickResult.h"

class USsProject;
class FSsAnimeDecoder;
class FSsCellMapList;


class SPRITESTUDIO5_API FSsPlayer
{
public:
	FSsPlayer();

	void SetSsProject(TWeakObjectPtr<USsProject> InSsProject);
	TWeakObjectPtr<USsProject> GetSsProject() { return SsProject; }

	FSsAnimeDecoder* GetDecoder() { return &*Decoder; }

	FSsPlayerTickResult Tick(float DeltaSeconds);
	void Draw(FCanvas* Canvas, FVector2D CenterLocation, float Rotation=0.f, FVector2D Scale=FVector2D(1.f,1.f), bool bMask=false);


	// 再生
	bool Play(int32 InAnimPackIndex, int32 InAnimationIndex, int32 StartFrame=0, float PlayRate=1.f, int32 LoopCount=0, bool bRoundTrip=false);
	// 一時停止
	void Pause(){ bPlaying = false; }
	// 再開
	bool Resume();
	// 再生中か取得
	bool IsPlaying() const { return bPlaying; }
	// アニメーション名からインデックスを取得
	bool GetAnimationIndex(const FName& InAnimPackName, const FName& InAnimationName, int32& OutAnimPackIndex, int32& OutAnimationIndex);

	// 指定フレーム送り
	void SetPlayFrame(float Frame);
	// 現在フレーム取得
	float GetPlayFrame() const;
	// 最終フレーム取得
	float GetAnimeEndFrame() const;

	// コマ送り（前フレームへ）
	void ToPrevFrame(bool bLoop=true);
	// コマ送り（次フレームへ）
	void ToNextFrame(bool bLoop=true);


	// パーツ名からインデックスを取得
	int32 GetPartIndexFromName(FName PartName) const;

	// パーツのTransformを取得(Canvas内2D座標系)
	FTransform GetPartTransformInCanvas(int32 PartIndex, int32 CanvasWidth, int32 CanvasHeight, FVector2D CenterLocation, float Rotation, FVector2D Scale) const;

private:
	void TickAnimation(float DeltaSeconds, FSsPlayerTickResult& Result);
	void FindUserDataInInterval(FSsPlayerTickResult& Result, float Start, float End);

public:
	float PlayRate;		// 再生速度
	int32 LoopCount;	// ループ回数. 0以下の場合は無限ループ. 往復再生の場合は片道で１回とみなす. 
	bool bRoundTrip;	// 往復再生
	bool bFlipH;		// 左右反転
	bool bFlipV;		// 上下反転
	TMap<int32, TWeakObjectPtr<UTexture>> TextureReplacements;	// パーツ毎のテクスチャ差し替え

	const FVector2D& GetAnimPivot() const { return AnimPivot; }	// アニメーションに設定されたPivotを取得

private:
	TWeakObjectPtr<USsProject> SsProject;
	TSharedPtr<FSsAnimeDecoder> Decoder;
	TSharedPtr<FSsCellMapList> CellMapList;
	bool bPlaying;
	bool bFirstTick;
	FVector2D AnimPivot;
};
