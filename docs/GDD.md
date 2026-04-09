# CovertOps1 Multiplayer Game Design Document (Implementation Baseline)

## Product Pillars
- Fast, readable 6v6 combat inspired by Black Ops 1 pacing.
- Server-authoritative multiplayer fairness with dedicated servers.
- Near-future original setting (2030) across five three-lane maps.
- Competitive mode depth: TDM, CTF, Search and Destroy.

## Implemented Foundation (This Revision)
- Multiplayer C++ layer under `Source/CovertOps1/Multiplayer`.
- Replicated match/game/player state classes.
- GAS bootstrap (ability system component + combat attribute set).
- Mode classes: `ACOTDMGameMode`, `ACOCTFGameMode`, `ACOSnDGameMode`.
- Objective actor framework for CTF and SnD objective loops.
- Hitscan weapon base class with distance-based damage falloff.
- Player loadout/perk/scorestreak replicated data contracts.
- Backend API v1 skeleton in `services/backend`.

## Core Rules Targets
- TTK target: ~0.30 to 0.40 seconds close range.
- Match size target: 6v6.
- TDM victory: score limit or time expiry highest score.
- CTF victory: capture limit or time expiry highest captures.
- SnD victory: first to 6 rounds.

## Perk Tiers
- Tier 1: Sprinter, Forager, Cold-Blooded, Blast Shield, Tactician.
- Tier 2: Penetrator, Sharpshooter, Steady Hands, Quickload, Tinkerer.
- Tier 3: Gas Mask, Endurance, Silent Step, Last Resort, Infiltrator.

## Scorestreak Targets
- Recon Drone (300), RC Detonator (400), Precision Strike (500), Attack Chopper (700), Guard Dog (900), Gunship (1100).

## Progression
- XP from kills, objective actions, and match completion.
- Credits earned from matches and challenges.
- Unlock flow for weapons/attachments/perks/streaks is data-driven via backend profile state.
