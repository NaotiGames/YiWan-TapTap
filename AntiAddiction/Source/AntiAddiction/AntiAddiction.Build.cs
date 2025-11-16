// Some copyright should be here...

using System.Collections.Generic;
using UnrealBuildTool;
using System.IO;
using System.Linq;
#if UE_5_0_OR_LATER
using EpicGames.Core;
#elif UE_4_26_OR_LATER
using Tools.DotNETCommon;
#endif

public class AntiAddiction : ModuleRules
{
	public AntiAddiction(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		string configFile = Path.Combine(PluginDirectory, "Content/Assets/**");
		RuntimeDependencies.Add(configFile);

		FileReference fileRef = new FileReference(Path.Combine(PluginDirectory, Name + ".uplugin"));
		PluginInfo plugin = new PluginInfo(fileRef, PluginType.Project);
		PublicDefinitions.Add(Name + "_UE_VERSION_NUMBER=TEXT(\"" + plugin.Descriptor.Version + "\")");
		PublicDefinitions.Add(Name + "_UE_VERSION=TEXT(\"" + plugin.Descriptor.VersionName + "\")");

		PluginInfo Info = Plugins.ReadProjectPlugins(Target.ProjectFile.Directory).First(x => x.Name == "TapLogin");
		PrivateIncludePaths.AddRange(
			new string[]
			{
				Info.Directory + "/Source/TapLogin/Private/",
				Info.Directory + "/Source/TapLogin/Public/",
				Path.Combine(ModuleDirectory, "Private"),
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"InputCore",
				"TapCommon",
				"Json",
				"HTTP",
				"JsonUtilities",
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"UMG",
				"TapLogin",
				"EngineSettings",
			}
		);

		if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			PublicAdditionalFrameworks.Add(
				new Framework(
					"AntiAddictionService",
					"../ThirdParty/iOS/Frameworks/AntiAddictionService.zip"
				)
			);

			PublicAdditionalFrameworks.Add(
				new Framework(
					"AntiAddictionUI",
					"../ThirdParty/iOS/Frameworks/AntiAddictionUI.zip",
					"AntiAdictionResources.bundle"
				)
			);
		}

		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"Launch"
				}
			);
			AdditionalPropertiesForReceipt.Add(
				"AndroidPlugin",
				Path.Combine(ModuleDirectory, "AntiAddiction_Android_UPL.xml")
			);
		}
	}
}