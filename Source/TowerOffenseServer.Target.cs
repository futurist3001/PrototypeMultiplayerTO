using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class TowerOffenseServerTarget : TargetRules
{
    public TowerOffenseServerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;
        //DefaultBuildSettings = BuildSettingsVersion.V2;
        DefaultBuildSettings = BuildSettingsVersion.V4;

        //ExtraModuleNames.Add("TowerOffense");
        ExtraModuleNames.AddRange(new string[] { "TowerOffense" });

        bUseChecksInShipping = true;
    }
}