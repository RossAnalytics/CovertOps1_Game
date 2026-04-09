# CovertOps1 Multiplayer Module

This project contains the Unreal Engine 5.7 game module and backend service scaffold for the CovertOps1 multiplayer initiative.

## What Was Implemented
- `Source/CovertOps1/Multiplayer` C++ foundation:
  - Authoritative multiplayer mode framework (TDM, CTF, SnD classes)
  - Replicated game/player state contracts
  - GAS bootstrap (AbilitySystem + replicated combat attributes)
  - Replicated character movement states (sprint/slide/prone) and stamina
  - Hitscan weapon base class with damage dropoff
  - Spawn policy component for anti-camp spawn selection
- Dedicated server build target: `Source/CovertOps1Server.Target.cs`
- EOS and OnlineSubsystem scaffolding in project/plugin/config setup
- Backend service at `services/backend` with required REST v1 endpoints
- Documentation in `docs/` and map layout notes in `docs/map-layouts`
- Build/bootstrap scripts under `scripts/`

## Installation
1. Install Unreal Engine 5.7.
2. Set `UE_ENGINE_ROOT` to your Unreal installation path.
3. Install Node.js 20+.
4. Run:
```powershell
pwsh .\scripts\bootstrap-dev.ps1
```

## Build
### Client
```powershell
pwsh .\scripts\build-client.ps1
```

### Dedicated Server
```powershell
pwsh .\scripts\build-server.ps1
```

### Backend
```powershell
pwsh .\scripts\run-backend.ps1
```

## Primary Docs
- `docs/GDD.md`
- `docs/networking-architecture.md`
- `docs/test-plan.md`
- `docs/platform-compatibility.md`
- `docs/art-direction-2026.md`
- `docs/ui-ux-direction-2026.md`
