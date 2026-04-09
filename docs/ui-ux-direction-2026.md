# CovertOps1 UI/UX Direction (2026 Competitive FPS)

## UX Principles
- Zero ambiguity during combat.
- Information hierarchy favors survivability decisions over cosmetic detail.
- Inputs and state changes must provide immediate confirmation (audio + visual + haptic-ready events).

## HUD Rules
- Minimap:
  - Persistent in top-left with distinct friendly/enemy/objective iconography.
  - UAV and jammer states visually unique.
- Center screen:
  - Hit markers and kill confirmation use brief, high-contrast bursts.
  - Damage direction indicators prioritize cardinal readability.
- Lower-right combat block:
  - Ammo, lethal, tactical, and streak readiness aligned in one glance path.
- Objective state:
  - Always-visible concise status text (flag state, bomb state, timer pressure).

## Scoreboard And Meta UX
- Scoreboard defaults:
  - K / D / A / Objective / Score / Ping.
  - SnD includes round-by-round result strip.
- Lobby:
  - Fast class editing loop, no deep nesting.
  - Party status and voice channels visible without modal friction.

## Visual Style
- Typography: clean geometric sans, high x-height, optimized for motion blur scenarios.
- Color coding:
  - Friendly: cool cyan/blue family.
  - Enemy: warm orange/red family.
  - Objective neutral: amber/white.
- Effects:
  - Keep motion restrained and functional.
  - No full-screen overlays that block target acquisition.

## Accessibility Requirements
- Colorblind-safe palette variants for team/objective indicators.
- Adjustable HUD scale and opacity.
- Subtitle + radio callout transcript support.
- Input remapping parity between keyboard/mouse and controller.
