# CovertOps1 UE5 Quickstart (Open Project + New MP Maps)

## 1) Open the project in Unreal Engine 5.7
1. Launch Epic Games Launcher.
2. Start **Unreal Engine 5.7**.
3. In UE, click **Browse...** and open:
   - `C:\Users\yeise\OneDrive\Documents\Unreal Projects\CovertOps1\CovertOps1.uproject`

## 2) Find the generated multiplayer maps
1. Open **Content Drawer**.
2. Go to:
   - `/Game/Multiplayer/Maps`
3. You should see:
   - `Lvl_MP_NeonDistrict`
   - `Lvl_MP_IndustrialComplex`
4. Double-click either map to open it.

## 3) Assign multiplayer GameMode for testing
For quick checks in-editor:
1. Open **Window -> World Settings**.
2. In **GameMode Override**, choose one of:
   - `ACOTDMGameMode` (Team Deathmatch)
   - `ACOCTFGameMode` (Capture the Flag)
   - `ACOSnDGameMode` (Search and Destroy)

## 4) Validate objectives and spawn points
Each generated map already includes:
- `18` PlayerStarts (for 6v6+ headroom)
- `2` CTF flags + `2` capture zones
- `1` SnD bomb actor + `2` bomb sites (A/B)
- lane callout text actors for navigation

## 5) Run a quick multiplayer PIE test
1. In toolbar Play settings, set:
   - **Number of Players**: `2` to `6`
   - **Net Mode**: `Play As Listen Server`
2. Click **Play**.
3. Confirm:
   - players spawn in protected zones
   - CTF flags can be picked/captured
   - SnD bomb actor and bomb sites are present

## 6) Regenerate maps from script (optional)
If you want to rebuild both maps from scratch:

```powershell
& 'C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe' \
  'C:\Users\yeise\OneDrive\Documents\Unreal Projects\CovertOps1\CovertOps1.uproject' \
  -unattended -nop4 -nosplash -nullrhi -run=pythonscript \
  -script='C:\Users\yeise\OneDrive\Documents\Unreal Projects\CovertOps1\scripts\unreal\generate_two_greybox_mp_maps.py'
```

Note: commandlet output currently reports EOS initialization errors when EOS credentials are not configured. That does not block map generation.