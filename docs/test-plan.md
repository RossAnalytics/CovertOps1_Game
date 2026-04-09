# CovertOps1 Multiplayer Test Plan (Baseline)

## Automated
- Unit/functional tests for:
  - Hitscan damage drop-off correctness.
  - Scorestreak unlock thresholds from score.
  - Mode victory conditions (TDM/CTF/SnD).
  - Perk selection replication integrity.

## Dedicated Server Integration
- Nightly headless session tests:
  - 12-player simulated matches.
  - Network emulation profiles: 30ms, 80ms, 150ms RTT.
  - Packet loss profiles: 0%, 1%, 3%.
- Validate:
  - Match start/end correctness.
  - No duplicate eliminations.
  - Respawn policy compliance by mode.

## Manual QA
- Core combat feel:
  - ADS/hipfire consistency.
  - Sprint and stance transitions.
  - Damage feedback and killfeed visibility.
- Objective loops:
  - Flag pickup/return/capture.
  - Bomb plant/defuse and round transitions.

## Anti-Cheat Validation
- Verify server-side event sanity checks:
  - Impossible fire cadence.
  - Out-of-bounds movement.
  - Invalid score bursts.
- Verify reporting flow end-to-end via backend `POST /reports/player`.

## Release Gates
- Match completion success rate >= 99% over soak tests.
- No blocker severity desync issues.
- Backend API latency p95 under 200ms in regional test environment.
