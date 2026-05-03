# Kilnseed P1 — Editor Setup Guide

All C++ is in place. This guide covers everything you need to create in-editor to get the game running: walking around, O2 draining outside the safe zone, dying and respawning.

---

## 1. Enhanced Input Assets

Create all assets in `Content/Kilnseed/Input/`.

### 1a. Input Actions (7 assets)

Right-click in Content Browser > Input > Input Action for each:

| Asset Name | Value Type | Notes |
|------------|-----------|-------|
| `IA_Move` | Axis2D (Vector2D) | WASD movement |
| `IA_Look` | Axis2D (Vector2D) | Mouse look |
| `IA_Jump` | Digital (Bool) | Space bar |
| `IA_Sprint` | Digital (Bool) | Shift (hold) |
| `IA_Interact` | Digital (Bool) | E key |
| `IA_PrimaryAction` | Digital (Bool) | Left mouse button |
| `IA_BuildMenu` | Digital (Bool) | B key |

For each: double-click to open, set the Value Type, save.

### 1b. Input Mapping Context (1 asset)

Right-click > Input > Input Mapping Context. Name it `IMC_Default`.

Double-click to open and add these mappings:

**IA_Move:**
- W → Modifiers: Swizzle Input Axis Values (YXZ), then this gives forward
- S → Modifiers: Swizzle Input Axis Values (YXZ), Negate
- D → (no modifiers needed, gives positive X)
- A → Modifiers: Negate

**IA_Look:**
- Mouse XY 2D-Axis → Modifiers: Negate (Y axis only, so pitch isn't inverted — add a "Negate" modifier and set Y to true, X to false)

**IA_Jump:**
- Space Bar → (no modifiers)

**IA_Sprint:**
- Left Shift → (no modifiers)

**IA_Interact:**
- E → (no modifiers)

**IA_PrimaryAction:**
- Left Mouse Button → (no modifiers)

**IA_BuildMenu:**
- B → (no modifiers)

Save.

---

## 2. Gameplay Effects

Create assets in `Content/Kilnseed/Data/Effects/`.

### 2a. GE_O2Drain

Right-click > Blueprint Class > search "GameplayEffect" > select GameplayEffect > name it `GE_O2Drain`.

Double-click to open. Set:

- **Duration Policy:** Infinite
- **Period:** 0.25

Under **Modifiers**, click + to add one:
- **Attribute:** KilnseedPlayerAttributeSet.O2Level
- **Modifier Op:** Add
- **Modifier Magnitude > Magnitude Calculation Type:** Attribute Based
- **Backing Attribute > Attribute to Capture:** KilnseedPlayerAttributeSet.O2DrainRate
- **Attribute Source:** Source
- **Coefficient:** -1.0

The drain rate is computed in C++ from O2MaxDuration (default 60s). Coefficient -1 makes it subtract.

Save.

### 2b. GE_O2Refill

Same process. Name it `GE_O2Refill`.

- **Duration Policy:** Infinite
- **Period:** 0.25

**Modifier:**
- **Attribute:** KilnseedPlayerAttributeSet.O2Level
- **Modifier Op:** Add
- **Magnitude Calculation Type:** Attribute Based
- **Backing Attribute > Attribute to Capture:** KilnseedPlayerAttributeSet.O2DrainRate
- **Attribute Source:** Source
- **Coefficient:** 3.0 (refills 3x faster than drain)

Save.

---

## 3. BP_SafeZone

Create in `Content/Kilnseed/Blueprints/Stations/`.

Right-click > Blueprint Class > All Classes > search `SafeZoneVolume` > select it > name `BP_SafeZone`.

Double-click to open. In the Details panel (Class Defaults):
- **O2 Drain Effect:** GE_O2Drain
- **O2 Refill Effect:** GE_O2Refill

Save. The volume size (800x800x600 units) is already set in C++.

---

## 4. BP_KilnseedPlayer

Create in `Content/Kilnseed/Blueprints/Player/`.

Right-click > Blueprint Class > All Classes > search `KilnseedPlayerCharacter` > select it > name `BP_KilnseedPlayer`.

Double-click to open. In Class Defaults, assign:

**Input section:**
- Default Mapping Context: `IMC_Default`
- IA_Move: `IA_Move`
- IA_Look: `IA_Look`
- IA_Jump: `IA_Jump`
- IA_Sprint: `IA_Sprint`
- IA_Interact: `IA_Interact`
- IA_PrimaryAction: `IA_PrimaryAction`
- IA_BuildMenu: `IA_BuildMenu`

**Abilities section:**
- Interact Ability Class: `GA_Interact` (C++ class, should appear in dropdown)

Save.

---

## 5. BP_KilnseedGameMode

Create in `Content/Kilnseed/Blueprints/`.

Right-click > Blueprint Class > All Classes > search `KilnseedGameMode` > select it > name `BP_KilnseedGameMode`.

Double-click to open. In Class Defaults:
- **Default Pawn Class:** `BP_KilnseedPlayer`
- **Player State Class:** `KilnseedPlayerState` (C++ class)
- **Game State Class:** `KilnseedGameState` (C++ class)
- **Default Abilities:** Click + and add `GA_Interact`

Save.

---

## 6. Create L_MainPlanet Level

File > New Level > Empty Level. Save as `Content/Kilnseed/Maps/L_MainPlanet`.

### 6a. Lighting & Sky

1. **Directional Light** — Drag from Place Actors panel. Set:
   - Rotation: (-40, -30, 0) for angled sunlight
   - Intensity: 3.0
   - Light Color: warm orange-ish (R:1.0, G:0.85, B:0.7)

2. **Sky Atmosphere** — Place Actors > search "Sky Atmosphere" > drag into level. Defaults are fine for now (we'll make it Mars-like later by tweaking Rayleigh scattering).

3. **Exponential Height Fog** — Place Actors > drag in. Set:
   - Fog Density: 0.03
   - Fog Inscattering Color: (R:0.6, G:0.3, B:0.2) — dusty red
   - Fog Height Falloff: 0.5

4. **SkyLight** — Place Actors > drag in. Set to "SLS Captured Scene" so it picks up the sky atmosphere.

### 6b. Ground Plane

For now, a simple floor to walk on:
1. Place Actors > Basic > Cube
2. Scale to (50, 50, 0.1) — this gives a 5000x5000cm flat surface
3. Position at (0, 0, 0)
4. Set material to default or a grey material

(We'll replace this with a proper Landscape later.)

### 6c. Safe Zone

1. Drag `BP_SafeZone` from Content Browser into the level
2. Position at (0, 0, 0) — center of the map
3. The volume is already sized (800x800x600). Players inside won't drain O2.

### 6d. Player Start

1. Place Actors > Basic > Player Start
2. Position at (0, 0, 100) — inside the safe zone so player spawns safe

### 6e. Greybox Hub (optional but nice)

1. Place Actors > Basic > Sphere
2. Scale to (4, 4, 2) — hemisphere-ish shape
3. Position at (0, 0, 0) — sits on the ground at center
4. This visually marks the safe zone / habitat hub

---

## 7. Project Settings

### 7a. Default Map & GameMode

Edit > Project Settings > Maps & Modes:
- **Default GameMode:** `BP_KilnseedGameMode`
- **Editor Startup Map:** `L_MainPlanet`
- **Game Default Map:** `L_MainPlanet`

### 7b. Verify Enhanced Input

Edit > Project Settings > Input:
- **Default Player Input Class** should already be `EnhancedPlayerInput`
- **Default Input Component Class** should already be `EnhancedInputComponent`

(These were set when the project was created.)

---

## 8. Test It

1. Press Play (PIE)
2. You should spawn at the Player Start inside the safe zone
3. WASD to move, mouse to look, space to jump, shift to sprint
4. Walk outside the safe zone volume — O2 should start draining (check with `showdebug abilitysystem` in the console: tilde `~` key)
5. Walk back inside — O2 should refill
6. Stay outside until O2 hits 0 — you should die and respawn at hub after 3 seconds

### Debug Tips
- **Console:** `showdebug abilitysystem` — shows active GEs, attribute values, tags
- **Console:** `show collision` — shows the safe zone volume boundaries
- If O2 doesn't drain, check that BP_SafeZone has GE_O2Drain/Refill assigned
- If movement doesn't work, check that BP_KilnseedPlayer has the IMC and IA_ assets assigned
- If GA_Interact doesn't exist in dropdowns, rebuild the project first

---

## Summary of Assets Created

```
Content/Kilnseed/
  Input/
    IA_Move, IA_Look, IA_Jump, IA_Sprint,
    IA_Interact, IA_PrimaryAction, IA_BuildMenu
    IMC_Default
  Data/Effects/
    GE_O2Drain, GE_O2Refill
  Blueprints/
    BP_KilnseedGameMode
    Player/BP_KilnseedPlayer
    Stations/BP_SafeZone
  Maps/
    L_MainPlanet
```

Total: 13 assets. ~20 minutes of editor work.

---
---

# Kilnseed P2 — Editor Setup (Farming Loop)

After P1 is working (walking + O2), add these for the plant/water/harvest loop.

---

## 1. Gameplay Effects for Plots

Create in `Content/Kilnseed/Data/Effects/`.

### 1a. GE_PlotGrowth

Blueprint Class > GameplayEffect > name `GE_PlotGrowth`.

- **Duration Policy:** Infinite
- **Period:** 0.25

**Modifier:**
- **Attribute:** KilnseedPlotAttributeSet.GrowthProgress
- **Modifier Op:** Add
- **Magnitude Calculation Type:** Set By Caller
- **Data Tag:** `Seed.Data.GrowthRate`

The per-tick growth rate is computed in C++ from each plant's `GrowthSeconds` data asset field (Aerolume=25s, Loamspine=50s, Tidefern=100s).

### 1b. GE_WaterDrain

Blueprint Class > GameplayEffect > name `GE_WaterDrain`.

- **Duration Policy:** Infinite
- **Period:** 0.25

**Modifier:**
- **Attribute:** KilnseedPlotAttributeSet.WaterLevel
- **Modifier Op:** Add
- **Magnitude Calculation Type:** Set By Caller
- **Data Tag:** `Seed.Data.WaterDrain`

The per-tick drain is computed in C++ from each plant's `WaterDrainRate` data asset field.

---

## 2. Blueprint Stations

Create all in `Content/Kilnseed/Blueprints/Stations/`.

### 2a. BP_Plot

Blueprint Class > PlotActor > name `BP_Plot`.

In Class Defaults:
- **Growth Effect:** GE_PlotGrowth
- **Water Drain Effect:** GE_WaterDrain

In the Components panel:
- **Mesh** (inherited): Assign a basic Cube mesh, scale to (1.5, 1.5, 0.1) — flat plot tile

Plant visuals are fully procedural — **do not add a plant mesh**. The `PlantVisualComponent` builds composite shapes at runtime from basic engine primitives (spheres, cubes, cones, cylinders) based on the plant type. Each plant type has a unique multi-part silhouette with staggered growth stages and per-part emissive color variation:
- **Aerolume** (lime): Bioluminescent cluster — stem + central bulb + orbiting pods + bloom cap
- **Loamspine** (amber): Crystal spire — central column + radiating spike cones + crown
- **Tidefern** (teal): Aquatic mushroom — dual stems + broad flat cap + trailing tendrils + bud orbs

### 2b. BP_TerraformHub

Blueprint Class > TerraformHubActor > name `BP_TerraformHub`.

In the Components panel:
- **Mesh** (inherited): Assign a Cube mesh, scale to (1.5, 1.5, 2.0) — tall box
- **AtmoIntake** (inherited): Appears as a small box on the left — assign a Cube mesh. Set material to a lime-tinted color to indicate Atmosphere intake.
- **SoilIntake** (inherited): Center box — assign a Cube mesh. Amber-tinted for Soil.
- **HydroIntake** (inherited): Right box — assign a Cube mesh. Teal-tinted for Hydrosphere.

No Class Defaults needed — delivery logic is fully C++. The hub accepts harvest crates via LMB and routes them to the correct terraform axis based on the crate's PlantType tag.

### 2c. BP_SeedDispenser


' '
Blueprint Class > SeedDispenserActor > name `BP_SeedDispenser`.

In Class Defaults:h
- **Seed Pod Class:** Set to the carriable class (ACarriableBase or a BP subclass — see 2d)
- **Available Plants:** Add entries for each plant DA asset (see step 3)
- **Dispense Cooldown:** 5.0
- **Power Draw:** 3.0

Components:
- **Mesh**: Assign a Cylinder mesh, scale to taste

### 2d. BP_WaterReservoir

Blueprint Class > WaterReservoirActor > name `BP_WaterReservoir`.

Components:
- **Mesh**: Assign a Cylinder mesh (larger, blue-tinted)

### 2e. BP_SeedPod, BP_WaterCanister, BP_HarvestCrate (optional)

For distinct visual items, create Blueprint subclasses of `CarriableBase`:
- **BP_SeedPod**: Sphere mesh, small (Scale 0.15), emissive material
- **BP_WaterCanister**: Cylinder mesh, small, blue material
- **BP_HarvestCrate**: Cube mesh, small (Scale 0.25), emissive material

Or just use the base CarriableBase — it works, just looks like a default mesh.

---

## 3. Plant Data Assets

Create in `Content/Kilnseed/Data/Plants/`.

Right-click > Miscellaneous > Data Asset > pick `PlantDataAsset` for each:

### DA_Aerolume
- Plant Id: `aerolume`
- Display Name: `Aerolume`
- Plant Color: (0.5, 0.9, 0.2, 1) — Lime Green
- Growth Day Cycles: 0.5 (half a day cycle)
- Water Drain Rate: 0.033
- Pollination Window: 15
- Terraform Axis: (leave blank for now, used in P3)
- Seed Dispense Cooldown: 5
- Plant Tag: `Seed.Plant.Aerolume`

### DA_Loamspine
- Plant Id: `loamspine`
- Display Name: `Loamspine`
- Plant Color: (0.9, 0.6, 0.2, 1) — Amber
- Growth Day Cycles: 1.0 (one full day cycle)
- Water Drain Rate: 0.033
- Pollination Window: 15
- Plant Tag: `Seed.Plant.Loamspine`

### DA_Tidefern
- Plant Id: `tidefern`
- Display Name: `Tidefern`
- Plant Color: (0.2, 0.8, 0.6, 1) — Teal
- Growth Day Cycles: 2.0 (two full day cycles)
- Water Drain Rate: 0.033
- Pollination Window: 15
- Plant Tag: `Seed.Plant.Tidefern`

---

## 4. Update BP_KilnseedGameMode

Open `BP_KilnseedGameMode` and add to **Default Abilities:**
- GA_Interact (already there from P1)
- GA_Pickup
- GA_Place
- GA_Harvest
- GA_ManualPollinate

---

## 5. Update BP_KilnseedPlayer

Open `BP_KilnseedPlayer` and set:
- **Pickup Ability Class:** GA_Pickup
- **Place Ability Class:** GA_Place
- **Harvest Crate Class:** `CarriableBase` (or `BP_HarvestCrate` if you created one)
- **IA_Flashlight:** `IA_Flashlight` (if created — see P1 extras below)
- **Sprint Drain Effect:** `GE_SprintO2Drain` (if created — see P1 extras below)

---

## 6. Place in Level

Open `L_MainPlanet` and add:

1. **3×3 Plot Grid**: Drag 9x `BP_Plot` into the level. Space them ~200 units apart in a 3×3 grid, offset ~800 units from center (south of hub).

2. **Seed Dispenser**: Drag `BP_SeedDispenser` into the level. Place east of hub (~500 units). In its Details:
   - **Seed Pod Class:** `CarriableBase` (or `BP_SeedPod` if created)
   - **Available Plants:** Click + and add `DA_Aerolume`

3. **Water Reservoir**: Drag `BP_WaterReservoir` into the level. Place west of hub. In its Details:
   - **Water Canister Class:** `CarriableBase` (or `BP_WaterCanister` if created)

4. **Terraform Hub** (optional for P2, required for P3): Drag `BP_TerraformHub` into the level. Place north of hub.

5. **Day/Night Cycle**: Place Actors > All Classes > search `DayNightCycleActor` > drag into level. Defaults: 60s day, 20s night. No configuration needed — it auto-finds the Directional Light.

6. **Post Process Volume**: Place Actors > Post Process Volume. Enable **Infinite Extent**. Under Exposure:
   - Metering Mode: Manual
   - Min EV100: 10.0
   - Max EV100: 10.0

---

## 7. P1 Extras (optional but recommended)

These assets are created during P1 polish but not strictly required:

**GE_SprintO2Drain** — Create in `Content/Kilnseed/Data/Effects/`:
- Duration Policy: Infinite
- Period: 0.25
- Modifier: Attribute Based on O2DrainRate, Coefficient: -2.0
- Assign in BP_KilnseedPlayer > Sprint Drain Effect

**IA_Flashlight** — Create in `Content/Kilnseed/Input/`:
- Value Type: Digital (Bool)
- Add to IMC_Default: F key, no modifiers
- Assign in BP_KilnseedPlayer > IA_Flashlight

---

## 8. Test It

### Seed → Grow → Harvest loop
1. Walk to seed dispenser, LMB to cycle plants, E to take a seed
2. Walk to an empty plot, LMB → plant the seed
3. Composite plant visual grows from seed size with emissive color
4. Prompt shows `Growing... X% | Water: Y%`
5. At 50%, plot enters POLLINATING — pulsing light appears, prompt shows `[E] Pollinate`
6. Press E to pollinate — growth resumes (one-time gate)
7. At 100%, prompt shows `[E] Harvest` — press E to get an emissive harvest crate

### Water loop
1. Walk to water reservoir, press E → get a blue water canister
2. Walk to a growing plot, LMB → waters the plot (adds 50% water)
3. Water level shown in plot prompt

### Delivery (P3 preview)
1. Carry a harvest crate to the Terraform Hub
2. LMB → delivers the crate, advances the terraform axis

---

## P2 Assets Summary

```
Content/Kilnseed/
  Data/
    Effects/GE_PlotGrowth, GE_WaterDrain, GE_SprintO2Drain (optional)
    Plants/DA_Aerolume, DA_Loamspine, DA_Tidefern
  Input/
    IA_Flashlight (optional)
  Blueprints/Stations/
    BP_Plot, BP_SeedDispenser, BP_WaterReservoir
```

Total: 8-10 new assets. ~15 minutes of editor work.
