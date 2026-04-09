# CovertOps1 Art Direction (2026 Modern Combat)

## Creative North Star
- Visual feel: crisp readability of Black Ops 1 combat language, rebuilt with 2026-grade material fidelity, volumetrics, and lighting.
- Tone: grounded military thriller with stylized accent lighting for instant combat readability.
- Rule: gameplay silhouettes first, beauty pass second.

## Global Rendering Targets
- High-frequency detail only where players read tactical information (cover edges, lanes, objective spaces).
- Distinct material separation: painted metal, anodized alloys, wet concrete, woven tactical textiles, emissive signage.
- Lighting model:
  - Hard key lights for lane clarity.
  - Soft fill fog for depth layering.
  - Controlled bloom (never bloom-wash target silhouettes).
- Weather is gameplay-aware:
  - Rain adds reflections and audio tension, not full visibility collapse.
  - Fog adds depth cues, not sniper suppression across all lanes.
  - Sandstorm windows are timed and telegraphed.

## Map Visual Language

### Neon District
- Palette: cyan/magenta accents over asphalt black and steel blue.
- Signature surfaces: wet roads, holographic ad panels, reflective tram glass.
- Hero props: rain-slick kiosks, scaffold billboards, metro flood gates.

### Desert Bazaar
- Palette: warm limestone, sun-bleached fabric, cobalt signage accents.
- Signature surfaces: canvas awnings, dusty plaster, oxidized shutters.
- Hero props: cloth canopy shadows, rooftop dish clusters, market alley clutter.

### Industrial Complex
- Palette: graphite steel, hazard yellow, sodium-vapor amber.
- Signature surfaces: oil stains, worn anti-slip metal, misted pipework.
- Hero props: crane gantries, conveyor banks, welding spark volumes.

### Rainforest Research Base
- Palette: deep greens + sterile lab whites with bio-neon accents.
- Signature surfaces: humid foliage, condensation glass, brushed composite panels.
- Hero props: greenhouse chambers, specimen tanks, jungle watch towers.

### Offshore Platform
- Palette: storm gray, maritime blue, warning orange.
- Signature surfaces: salt-corroded steel, wet grating, painted safety rails.
- Hero props: wave spray emitters, crane cabins, engine core vents.

## Character And Weapon Art
- Operators:
  - Distinct faction readability at 20m, 40m, and 60m.
  - Gear breakup by shape language (plate carriers, headset rigs, shoulder profile).
- Weapons:
  - First-person view uses physically plausible machining and wear masks.
  - Distinct muzzle flash character per weapon family.
  - Attachment silhouettes recognizable in under 150ms glance time.

## VFX Quality Bar
- Impact FX uses layered sparks/debris/smoke with lane-safe opacity.
- Explosions have pressure + particulate + heat distortion phases.
- Scorestreak FX:
  - Recon Drone: scan sweep and subtle HUD pulse.
  - Attack Chopper/Gunship: directional tracer + rotor wash fog breakup.

## Technical Budgets (PC/Xbox-ready)
- Keep objective zones and lane intersections within predictable GPU cost windows.
- Use Nanite for structural environment assets; keep dynamic clutter optimized by lane importance.
- LOD and material instance strategy must preserve tactical silhouettes.
- Avoid overdraw-heavy particles in objective-heavy chokepoints.

## Asset Review Gate
- Every shippable asset passes:
  - `Readability`: enemy/cover/objective clear within 1 second.
  - `Cohesion`: palette and material language match map profile.
  - `Performance`: no local frame spikes above map budget.

## Neon District Gate Thresholds (M3 Alpha Baseline)
- Contrast: `1.00 - 1.35`
- Saturation: `0.95 - 1.30`
- Bloom intensity max: `0.45` (target `<= 0.34` for Neon District)
- Fog density max: `0.05` (target `~0.018` for rain-night readability)
- Rain requires wetness response: if rain > `0`, wetness >= `0.35`
- Night directional intensity (`TargetLuxNight`): `1 - 8`
- Accent palette minimum: `3` distinct lane-readable colors
