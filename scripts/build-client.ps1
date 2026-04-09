param(
    [string]$Configuration = "Development",
    [string]$Platform = "Win64",
    [string]$ProjectFile = "$PSScriptRoot\..\CovertOps1.uproject"
)

$ErrorActionPreference = "Stop"

$uprojectPath = (Resolve-Path $ProjectFile).Path
$engineBatch = "${env:UE_ENGINE_ROOT}\Engine\Build\BatchFiles\Build.bat"

if (!(Test-Path $engineBatch)) {
    throw "UE_ENGINE_ROOT is not set or Build.bat was not found. Set UE_ENGINE_ROOT to your UE 5.7 install path."
}

& $engineBatch CovertOps1 $Platform $Configuration $uprojectPath -NoHotReload
