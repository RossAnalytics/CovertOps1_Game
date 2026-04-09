# PC And Xbox Compatibility Strategy

## Current Technical Baseline
- Runtime code uses Unreal cross-platform APIs and avoids platform-locked rendering/input paths.
- Enhanced Input and controller support are active for PC gamepad parity with console input expectations.
- Online flow is built around EOS-compatible session patterns (`UCOMatchmakingSubsystem`).

## PC Shipping Track
- Target: Win64 first.
- Build route: `scripts/build-client.ps1`.
- Dedicated server target exists in code (`CovertOps1Server.Target.cs`), but requires an engine distribution that supports server targets.

## Xbox Readiness Track
- Use a source-capable Unreal Engine + Xbox GDK-enabled toolchain in a secured environment.
- Required workstreams:
  - Xbox platform packaging and TRC/TCR compliance passes.
  - Xbox Live/EOS account-linking and entitlement handling.
  - Controller-only UX validation and safe-area UI pass.
  - Performance budget tuning against target Xbox hardware tiers.

## Cross-Play Guardrails
- Keep authoritative server simulation identical across platforms.
- Enforce backend-driven progression to prevent platform divergence.
- Store platform identifiers and session metadata server-side for compliance reporting.
