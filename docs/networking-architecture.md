# CovertOps1 Networking Architecture (Baseline)

## Server Authority
- Dedicated server owns match state, score, round flow, eliminations, and respawn decisions.
- Clients submit intent (loadout/perks/input actions); server validates and commits replicated state.

## Unreal Runtime Topology
- `ACOMultiplayerGameMode`: authoritative mode logic and win conditions.
- `ACOMultiplayerGameState`: replicated timer, phase, team scores, rounds.
- `ACOMultiplayerPlayerState`: replicated loadout/perks/scorestreak state and stats.
- `ACOMultiplayerCharacter`: replicated stance, sprinting, stamina, network health.
- `UCovertOpsSpawnDirectorComponent`: spawn policy selection for anti-camp respawns.
- Objective actors:
  - `ACOFlagObjectiveActor` + `ACOFlagCaptureZoneActor` for CTF pickup/capture/reset flow.
  - `ACOBombObjectiveActor` + `ACOBombSiteActor` for SnD pickup/plant/defuse/explode flow.

## GAS Bootstrap
- `UCOMultiplayerAbilitySystemComponent` on player state.
- `UCOCombatAttributeSet` replicated attributes: health/max health/stamina/max stamina/accuracy.
- Perks/streak effects should be implemented as GameplayEffects/Abilities in follow-on milestones.

## Online Stack
- Online subsystem target: EOS.
- Current config includes EOS plugin enablement and subsystem defaults.
- `UCOMultiplayerGameInstance` logs active online subsystem at startup.
- `UCOMatchmakingSubsystem` provides host/find/join/destroy session flow with EOS-compatible lobby/session settings.

## Backend Integration Client
- `UCOBackendApiSubsystem` sends authenticated HTTP requests from Unreal runtime to backend REST v1.
- Supports matchmaking ticket requests, profile fetch/update, XP/credits grants, match result submission, and player reporting.

## Backend APIs
- Service path: `services/backend`.
- REST v1 implemented:
  - `POST /matchmaking/tickets`
  - `POST /match/results`
  - `GET|PUT /profiles/:playerId`
  - `POST /progression/xp`
  - `POST /economy/credits/grant`
  - `POST /reports/player`
- EOS auth middleware is scaffolded with dev bypass option for local iteration.

## Data Contracts
- Unreal replicated structs:
  - `FLoadoutSpec`
  - `FPerkSelection`
  - `FScorestreakState`
  - `FMatchResult`
- Backend mirrors equivalent JSON contracts for profile/match/persistence workflows.

## Objective Interaction Flow
- `ACOMultiplayerPlayerController::TryObjectiveInteract()` performs server-authoritative objective actions:
  - Pick up/deliver CTF flags.
  - Pick up bomb, plant at valid site, defuse planted bomb.
