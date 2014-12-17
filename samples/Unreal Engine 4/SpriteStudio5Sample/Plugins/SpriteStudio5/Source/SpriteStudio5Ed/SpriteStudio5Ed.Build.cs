// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class SpriteStudio5Ed : ModuleRules
	{
		public SpriteStudio5Ed(TargetInfo Target)
		{
			DynamicallyLoadedModuleNames.AddRange(
				new string[] {
					"PropertyEditor",
				}
				);

			PublicIncludePaths.AddRange(
				new string[] {
					"SpriteStudio5Ed/Public",
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					"SpriteStudio5Ed/Private",
					"SpriteStudio5Ed/Private/Factories",
					"SpriteStudio5Ed/Private/Loader",
					"SpriteStudio5Ed/Private/Loader/babel",
					"SpriteStudio5Ed/Private/Loader/tinyxml2",
					"SpriteStudio5Ed/Private/Viewer",
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"InputCore",
					"UnrealEd",
					"AssetTools",
					"Slate",
					"SlateCore",
					"EditorStyle",
//					"AppFramework",		// UE4.5以降でビルドする場合は、この行を有効にして下さい

					"SpriteStudio5",
				}
				);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
				}
				);

			DynamicallyLoadedModuleNames.AddRange(
				new string[]
				{
				}
				);
		}
	}
}