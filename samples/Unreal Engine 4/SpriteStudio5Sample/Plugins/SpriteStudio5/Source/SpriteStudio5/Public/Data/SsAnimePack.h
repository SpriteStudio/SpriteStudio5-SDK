#pragma once

#include "SsAttribute.h"

#include "SsAnimePack.generated.h"


/// アニメーション再生設定情報です。
USTRUCT()
struct SPRITESTUDIO5_API FSsAnimationSettings
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category=SsAnimationSettings, BlueprintReadOnly)
	int32	Fps;			//!< 再生FPS

	UPROPERTY(VisibleAnywhere, Category=SsAnimationSettings, BlueprintReadOnly)
	int32	FrameCount;		//!< フレーム数

	UPROPERTY(VisibleAnywhere, Category=SsAnimationSettings, BlueprintReadOnly)
	TEnumAsByte<SsPartsSortMode::Type>	SortMode;		//!< パーツのソートモード

	UPROPERTY(VisibleAnywhere, Category=SsAnimationSettings, BlueprintReadOnly)
	FVector2D	CanvasSize;				//!< キャンバスサイズ(元基準枠)。ビューポートのサイズとイコールではない。

	UPROPERTY(VisibleAnywhere, Category=SsAnimationSettings, BlueprintReadOnly)
	FVector2D	Pivot;					//!< キャンバスの原点。0,0 が中央。-0.5, +0.5 が左上
};



//パーツ一つあたりの情報を保持するデータです。
USTRUCT()
struct SPRITESTUDIO5_API FSsPart
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category=SsPart, BlueprintReadOnly)
	FName	PartName;

	UPROPERTY(VisibleAnywhere, Category=SsPart, BlueprintReadOnly)
	int32	ArrayIndex;		//!< ツリーを配列に展開した時のインデックス

	UPROPERTY(VisibleAnywhere, Category=SsPart, BlueprintReadOnly)
	int32	ParentIndex;	//!< 親パーツのインデックス

	UPROPERTY(VisibleAnywhere, Category=SsPart, BlueprintReadOnly)
	TEnumAsByte<SsPartType::Type>		Type;			//!< 種別

	UPROPERTY(VisibleAnywhere, Category=SsPart, BlueprintReadOnly)
	TEnumAsByte<SsBoundsType::Type>		BoundsType;		//!< 当たり判定として使うか？使う場合はその形状。

	UPROPERTY(VisibleAnywhere, Category=SsPart, BlueprintReadOnly)
	TEnumAsByte<SsInheritType::Type>	InheritType;	//!< アトリビュート値の継承方法

	UPROPERTY(VisibleAnywhere, Category=SsPart, BlueprintReadOnly)
	TEnumAsByte<SsBlendType::Type>		AlphaBlendType;	//!< αブレンドの演算式

	UPROPERTY(VisibleAnywhere, Category=SsPart, BlueprintReadOnly)
	int32	Show;			//!< [編集用データ] パーツの表示・非常時

	UPROPERTY(VisibleAnywhere, Category=SsPart, BlueprintReadOnly)
	int32	Locked;			//!< [編集用データ] パーツのロック状態

	UPROPERTY(VisibleAnywhere, Category=SsPart, BlueprintReadOnly)
	float InheritRates[(int)SsAttributeKind::Num];	///< 親の値の継承率。SS4との互換性のため残されているが0 or 1


	UPROPERTY(VisibleAnywhere, Category=SsPart, BlueprintReadOnly)
	FName	RefAnimePack;   ///< 参照アニメ名

	UPROPERTY(VisibleAnywhere, Category=SsPart, BlueprintReadOnly)
	FName	RefAnime;       ///< 参照アニメ名

public:
	FSsPart()
		: PartName(TEXT("")), ArrayIndex(0), ParentIndex(0), Show(0), Locked(0)
	{
		for (int i = 0; i < (int)SsAttributeKind::Num; ++i)
		{
			InheritRates[i] = 1.f;
		}
		// イメージ反転は継承しない
		InheritRates[(int)SsAttributeKind::Imgfliph] = 0.f;
		InheritRates[(int)SsAttributeKind::Imgflipv] = 0.f;
	}
};


//アニメーションを構成するパーツをリスト化したものです。
USTRUCT()
struct SPRITESTUDIO5_API FSsModel
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category=SsModel, BlueprintReadOnly)
	TArray<FSsPart>	PartList;	//!<格納されているパーツのリスト
};


//typedef std::vector<SsAttribute*>	SsAttributeList;

USTRUCT()
struct SPRITESTUDIO5_API FSsPartAnime
{
	GENERATED_USTRUCT_BODY()
	void Serialize(FArchive& Ar);

public:
	UPROPERTY(VisibleAnywhere, Category=SsPartAnime, BlueprintReadOnly)
	FName PartName;

	UPROPERTY(VisibleAnywhere, Category=SsPartAnime, BlueprintReadOnly)
	TArray<FSsAttribute> Attributes;
};


/// ラベル。ループ先などに指定する
USTRUCT()
struct SPRITESTUDIO5_API FSsLabel
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category=SsLabel, BlueprintReadOnly)
	FName		LabelName;	///< 名前 [変数名変更禁止]

	UPROPERTY(VisibleAnywhere, Category=SsLabel, BlueprintReadOnly)
	int32		Time;		///< 設置された時間(フレーム) [変数名変更禁止]
};


USTRUCT()
struct SPRITESTUDIO5_API FSsAnimation
{
	GENERATED_USTRUCT_BODY()
	void Serialize(FArchive& Ar);

public:
	UPROPERTY(VisibleAnywhere, Category=SsAnimation, BlueprintReadOnly)
	FName	AnimationName;					/// アニメーションの名称

	UPROPERTY(VisibleAnywhere, Category=SsAnimation, BlueprintReadOnly)
	bool	OverrideSettings;				/// このインスタンスが持つ設定を使いanimePack の設定を参照しない。FPS, frameCount は常に自身の設定を使う。

	UPROPERTY(VisibleAnywhere, Category=SsAnimation, BlueprintReadOnly)
	FSsAnimationSettings	Settings;		/// 設定情報

	UPROPERTY(Category=SsAnimation, BlueprintReadOnly)	// 要素数が多くDetailsウィンドウが極端に重くなってしまうため、VisibleAnywhereを付けない
	TArray<FSsPartAnime>	PartAnimes;		///	パーツ毎のアニメーションキーフレームが格納されるリスト

	UPROPERTY(VisibleAnywhere, Category=SsAnimation, BlueprintReadOnly)
	TArray<FSsLabel>		Labels;
};

/**
*@class SsAnimePack
*@brief パーツを組み合わせた構造とその構造を使用するアニメーションを格納するデータです。
パーツの組み合わせ構造をSsModel、Modelを使用するアニメデータをSsAnimationで定義しています。
*/
USTRUCT()
struct SPRITESTUDIO5_API FSsAnimePack
{
	GENERATED_USTRUCT_BODY()

	void Serialize(FArchive& Ar);

public:
	UPROPERTY(VisibleAnywhere, Category=SsAnimePack, BlueprintReadOnly)
	FString					Version;

	UPROPERTY(VisibleAnywhere, Category=SsAnimePack, BlueprintReadOnly)
	FSsAnimationSettings	Settings;		//!< 設定情報 

	UPROPERTY(VisibleAnywhere, Category=SsAnimePack, BlueprintReadOnly)
	FName					AnimePackName;	//!< アニメーションパック名称

	UPROPERTY(VisibleAnywhere, Category=SsAnimePack, BlueprintReadOnly)
	FSsModel				Model;			//!< パーツ情報の格納先

	UPROPERTY(VisibleAnywhere, Category=SsAnimePack, BlueprintReadOnly)
	TArray<FName>			CellmapNames;	//!< 使用されているセルマップの名称		

	UPROPERTY(VisibleAnywhere, Category=SsAnimePack, BlueprintReadOnly)
	TArray<FSsAnimation>	AnimeList;		//!< 格納されている子アニメーションのリスト

	// アニメーション名からインデックスを取得する
	int32 FindAnimationIndex(const FName& Name);
};
