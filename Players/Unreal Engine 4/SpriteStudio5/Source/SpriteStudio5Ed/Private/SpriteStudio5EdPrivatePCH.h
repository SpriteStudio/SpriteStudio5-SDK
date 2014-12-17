#include "SsVersion.h"

#include "Core.h"
#include "CoreUObject.h"
#include "Engine.h"
#include "InputCore.h"
#include "RenderCore.h"
#include "UnrealEd.h"
#include "AssetToolsModule.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/STextComboBox.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Colors/SColorPicker.h"		// UE4.5以降のビルドでココでエラーが出る場合は、SpriteStudio5Ed.Build.cs 44行目 PublicDependencyModuleNames を確認して下さい. 
#include "EditorStyle.h"

#include "ISpriteStudio5Ed.h"
#include "SpriteStudio5EdStyle.h"

#define SSPJ_COLOR FColor(154, 103, 168)



