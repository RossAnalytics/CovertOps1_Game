# Multiplayer Objective + Neon Visual Setup

## What Was Implemented
- `IA_Interact` created at `/Game/Variant_Shooter/Input/Actions/IA_Interact`.
- `IMC_Weapons` now maps interact to:
  - Keyboard: `E`
  - Gamepad/Xbox: `Gamepad_FaceButton_Left` (X button)
- Objective Blueprint children created:
  - `/Game/Multiplayer/Objectives/BP_COFlagObjectiveActor`
  - `/Game/Multiplayer/Objectives/BP_COBombObjectiveActor`
  - `/Game/Multiplayer/Objectives/BP_COBombSiteActor`
- Neon visual profile asset created:
  - `/Game/Multiplayer/Visual/DA_Visual_NeonDistrict`

## Interact Flow (Code)
- `ACOMultiplayerPlayerController` now binds `InteractAction` in `SetupInputComponent`.
- Press starts hold interaction (`TryObjectiveInteract`).
- Release/cancel stops hold interaction (`StopObjectiveInteract`).
- Server RPCs:
  - `ServerTryObjectiveInteract`
  - `ServerStopObjectiveInteract`
- Prompt/progress context is updated each tick on local controller and pushed to HUD.

## SnD Hold Plant/Defuse
- `ACOBombObjectiveActor` supports server-authoritative hold interactions:
  - Planting: `StartPlanting`
  - Defusing: `StartDefuse`
- Cancels automatically if the interacting pawn:
  - moves above threshold speed,
  - takes recent damage,
  - leaves valid interaction range/site.
- Interaction state is replicated:
  - `InteractionType`
  - `InteractionProgress`
  - `ActiveInteractor`
  - `ActiveInteractionSite`

## HUD Hooks To Implement In UMG
- `UCOMultiplayerHUDWidget::BP_UpdateObjectivePrompt(PromptText, bVisible)`
- `UCOMultiplayerHUDWidget::BP_UpdateObjectiveProgress(NormalizedProgress, bVisible, bIsDefuse)`
- Existing objective status and killfeed hooks remain active.

## Objective Presentation Hooks
- `COBombObjectiveActor`:
  - `BP_OnBombStateChanged`
  - `BP_OnBombInteractionChanged`
- `COFlagObjectiveActor`:
  - `BP_OnFlagStateChanged`

Use these Blueprint events to trigger production VFX/SFX/animation timelines.

## Neon Visual Pass
- New actor class: `ACONeonDistrictVisualDirector`
  - Applies `UCovertOpsVisualProfileData` values to post-process/fog/lighting.
  - Runs art-bible gate audit via `RunArtBibleGateAudit`.
- The Python setup script currently creates `DA_Visual_NeonDistrict`.
- `BP_CONeonDistrictVisualDirector` will be created by the same script after C++ is built and reloaded.

## Asset Script
- Script path:
  - `scripts/unreal/setup_multiplayer_objectives_and_neon_assets.py`
- Run from command line:
```powershell
"C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" `
  "C:\Users\yeise\OneDrive\Documents\Unreal Projects\CovertOps1\CovertOps1.uproject" `
  -run=pythonscript `
  -script="C:\Users\yeise\OneDrive\Documents\Unreal Projects\CovertOps1\scripts\unreal\setup_multiplayer_objectives_and_neon_assets.py" `
  -unattended -nop4 -nosplash
```

## Current Blocking Note
- `BP_COBombSiteActor` could not auto-assign `site_mesh` until the new `COBombSiteActor` C++ change is compiled/reloaded.
- `BP_CONeonDistrictVisualDirector` creation is also waiting on C++ compile/reload.
