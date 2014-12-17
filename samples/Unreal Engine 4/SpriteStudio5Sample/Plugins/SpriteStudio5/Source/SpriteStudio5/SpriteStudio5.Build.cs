// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class SpriteStudio5 : ModuleRules
	{
		public SpriteStudio5(TargetInfo Target)
		{
			DynamicallyLoadedModuleNames.AddRange(
				new string[] {
				}
				);

			PublicIncludePaths.AddRange(
				new string[] {
					"SpriteStudio5/Public",
					"SpriteStudio5/Public/Data",
					"SpriteStudio5/Public/HUD",
					"SpriteStudio5/Public/Player",
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					"SpriteStudio5/Private",
					"SpriteStudio5/Private/Component",
					"SpriteStudio5/Private/Data",
					"SpriteStudio5/Private/HUD",
					"SpriteStudio5/Private/Player",
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"InputCore",
					"RenderCore",
					"ShaderCore",
					"RHI",
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