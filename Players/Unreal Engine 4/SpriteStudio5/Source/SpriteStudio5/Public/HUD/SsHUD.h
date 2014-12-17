#pragma once


#include "SsHUD.generated.h"


UCLASS()
class SPRITESTUDIO5_API ASsHUD : public AHUD
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(Category=SpriteStudio, BlueprintCallable, meta=(AdvancedDisplay="2"))
	void DrawSsPlayer(class USsPlayerComponent* SsPlayer, FVector2D Location, float Rotation=0.f, FVector2D Scale=FVector2D(1.f,1.f));
};

