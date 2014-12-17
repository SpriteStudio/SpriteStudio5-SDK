#pragma once

#include "SsPlayerTickResult.generated.h"


//
// FSsPlayerの１フレーム分の更新結果
//
struct SPRITESTUDIO5_API FSsPlayerTickResult
{
	FSsPlayerTickResult()
		: bEndPlay(false)
	{}

	// 再生終了した
	bool bEndPlay;

	// 更新されたユーザーデータ
	TArray<struct FSsUserData> UserData;
};


USTRUCT()
struct SPRITESTUDIO5_API FSsUserDataRect
{
	GENERATED_USTRUCT_BODY()

	FSsUserDataRect()
		: Left(0)
		, Top(0)
		, Right(0)
		, Bottom(0)
	{}

	UPROPERTY(Category=SpriteStudioUserData, BlueprintReadWrite)
	int32 Left;

	UPROPERTY(Category=SpriteStudioUserData, BlueprintReadWrite)
	int32 Top;

	UPROPERTY(Category=SpriteStudioUserData, BlueprintReadWrite)
	int32 Right;

	UPROPERTY(Category=SpriteStudioUserData, BlueprintReadWrite)
	int32 Bottom;
};

USTRUCT(BlueprintType)
struct SPRITESTUDIO5_API FSsUserDataValue
{
	GENERATED_USTRUCT_BODY()

	FSsUserDataValue()
		: bUseInteger(false)
		, bUsePoint(false)
		, bUseRect(false)
		, bUseString(false)
		, Integer(0)
		, Point(0.f, 0.f)
	{}

	UPROPERTY(Category=SpriteStudioUserData, BlueprintReadWrite)
	bool bUseInteger;

	UPROPERTY(Category=SpriteStudioUserData, BlueprintReadWrite)
	bool bUsePoint;

	UPROPERTY(Category=SpriteStudioUserData, BlueprintReadWrite)
	bool bUseRect;

	UPROPERTY(Category=SpriteStudioUserData, BlueprintReadWrite)
	bool bUseString;

	UPROPERTY(Category=SpriteStudioUserData, BlueprintReadWrite)
	int32 Integer;

	UPROPERTY(Category=SpriteStudioUserData, BlueprintReadWrite)
	FVector2D Point;

	UPROPERTY(Category=SpriteStudioUserData, BlueprintReadWrite)
	FSsUserDataRect Rect;

	UPROPERTY(Category=SpriteStudioUserData, BlueprintReadWrite)
	FString String;
};

USTRUCT()
struct SPRITESTUDIO5_API FSsUserData
{
	GENERATED_USTRUCT_BODY()

	FSsUserData()
		: PartIndex(-1)
		, KeyFrame(-1)
	{}
	
	UPROPERTY(Category=SpriteStudioUserData, BlueprintReadWrite)
	FName PartName;

	UPROPERTY(Category=SpriteStudioUserData, BlueprintReadWrite)
	int32 PartIndex;

	UPROPERTY(Category=SpriteStudioUserData, BlueprintReadWrite)
	int32 KeyFrame;

	UPROPERTY(Category=SpriteStudioUserData, BlueprintReadWrite)
	FSsUserDataValue Value;
};
