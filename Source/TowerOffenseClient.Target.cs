using UnrealBuildTool;
using System.Collections.Generic;

public class TowerOffenseClientTarget : TargetRules
{
    public TowerOffenseClientTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Client;

        DefaultBuildSettings = BuildSettingsVersion.V4;

        ExtraModuleNames.AddRange(new string[] { "TowerOffense" });

        //TowerOffenseTarget.ApplySharedTowerOffenseTargetSettings(this);
    }
}
