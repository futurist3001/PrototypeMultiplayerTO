// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class TowerOffense : ModuleRules
{
	public TowerOffense(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore",
			"EnhancedInput", "UMG", "GeometryCollectionEngine", "Niagara", "Json", "JsonUtilities", "Landscape",
		"GameplayTasks", "AIModule", "OnlineSubsystem", "OnlineSubsystemSteam", "WebSockets"});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
