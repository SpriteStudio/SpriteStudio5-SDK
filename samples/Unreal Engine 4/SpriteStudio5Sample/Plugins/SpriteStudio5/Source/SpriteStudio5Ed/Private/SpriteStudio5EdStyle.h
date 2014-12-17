#pragma once

#include "SlateStyle.h"

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush(RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)

class FSpriteStudio5EdStyle : public FSlateStyleSet
{
public:
	FSpriteStudio5EdStyle()
		: FSlateStyleSet("SpriteStudio5EdStyle")

	{
		SetContentRoot(FPaths::GamePluginsDir() / TEXT("SpriteStudio5/Resources"));

		Set("PlayIcon", new IMAGE_BRUSH("play_icon", FVector2D(40.f, 40.f)));
		Set("PrevIcon", new IMAGE_BRUSH("prev_icon", FVector2D(40.f, 40.f)));
		Set("NextIcon", new IMAGE_BRUSH("next_icon", FVector2D(40.f, 40.f)));
		Set("GridIcon", new IMAGE_BRUSH("grid_icon", FVector2D(40.f, 40.f)));

		FSlateStyleRegistry::RegisterSlateStyle(*this);
	}

	~FSpriteStudio5EdStyle()
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*this);
	}

};

#undef IMAGE_BRUSH
