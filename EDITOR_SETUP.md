# Kilnseed — Editor Setup Guide

All C++ is in place. This guide covers the editor-side assets you need to create.

---

## Build System (P3)

### 1. Create IA_BuildSelect Input Action

In `Content/Kilnseed/Input/`:

Right-click > Input > Input Action. Name it `IA_BuildSelect`.
- **Value Type:** Axis1D (Float)

Open `IMC_Default` and add mappings for `IA_BuildSelect`:

| Key | Modifiers                |
|-----|--------------------------|
| 1   | (none — default 1.0)    |
| 2   | Scalar, Scale X = 2.0   |
| 3   | Scalar, Scale X = 3.0   |
| 4   | Scalar, Scale X = 4.0   |
| 5   | Scalar, Scale X = 5.0   |
| 6   | Scalar, Scale X = 6.0   |
| 7   | Scalar, Scale X = 7.0   |

Then open `BP_KilnseedPlayer` and set **IA_BuildSelect** to the new action.

### 2. Create Buildable Data Assets

Create all in `Content/Kilnseed/Data/Blueprints/`.

Right-click > Miscellaneous > Data Asset > pick `BlueprintDataAsset` for each:

#### DA_Plot
- **Blueprint Id:** `plot`
- **Display Name:** `Plot`
- **Actor Class:** `BP_Plot` (the blueprint from P2)
- **Resource Cost:** `0` (free — spawns directly, no ghost)
- **Requires Soil:** `true`

#### DA_WindTurbine
- **Blueprint Id:** `windturbine`
- **Display Name:** `Wind Turbine`
- **Actor Class:** `WindTurbineActor` (update to `BP_WindTurbine` after step 3)
- **Resource Cost:** `1`

#### DA_SolarPanel
- **Blueprint Id:** `solarpanel`
- **Display Name:** `Solar Panel`
- **Actor Class:** `SolarPanelActor` (update to `BP_SolarPanel` after step 3)
- **Resource Cost:** `2`

#### DA_Battery
- **Blueprint Id:** `battery`
- **Display Name:** `Battery`
- **Actor Class:** `BatteryActor` (update to `BP_Battery` after step 3)
- **Resource Cost:** `1`

### 3. Create Buildable Blueprints

Create all in `Content/Kilnseed/Blueprints/Stations/`.

#### BP_WindTurbine
Right-click > Blueprint Class > All Classes > search `WindTurbineActor`.

No configuration needed — the C++ constructor builds the full composite shape:
- Cylinder pole, cube nacelle, 3 spinning blade arms
- Blades rotate at a speed proportional to wind intensity
- Wind is strong early (thin atmosphere) and gusty/mild late

In Class Defaults (optional tweaks):
- **Watts Provided:** `5.0` (max output at full wind)

Go back to `DA_WindTurbine` and update **Actor Class** to `BP_WindTurbine`.

#### BP_SolarPanel
Right-click > Blueprint Class > All Classes > search `SolarPanelActor`.

No configuration needed — C++ builds the composite:
- Cylinder pole with a flat dark blue panel on a pivot
- Panel tracks the sun throughout the day cycle, rests flat at night
- Power output scales with atmosphere progress (10% at 0% atmo, 100% at full)

In Class Defaults (optional tweaks):
- **Watts Provided:** `10.0` (max output at full atmosphere)
- **Min Efficiency:** `0.1`

Go back to `DA_SolarPanel` and update **Actor Class** to `BP_SolarPanel`.

#### BP_Battery
Right-click > Blueprint Class > All Classes > search `BatteryActor`.

In Class Defaults:
- **Storage Capacity:** `50.0` (50 Watt-seconds of storage)

Go back to `DA_Battery` and update **Actor Class** to `BP_Battery`.

**Power system notes:**
- Wind turbines produce 0-5W depending on wind (strong early, gusty late)
- Solar panels produce 1-10W depending on atmosphere clarity
- Batteries store excess power and discharge during shortfalls
- HUD shows: demand/supply (grey=ok, amber=discharging, red=brownout), battery bar when capacity > 0
- Brownout (demand > supply + battery) halves plot growth speed

### 4. Starting Plots

Drag 3x `BP_Plot` into the level by hand (south of hub, ~200 units apart). These starter plots bypass the build system. Additional plots require soil terraform progress — placeable radius expands as you deliver loamspine.

### 5. Update BP_KilnseedGameMode

Open `BP_KilnseedGameMode`. In Class Defaults:

**Default Abilities** — add (in addition to existing):
- `GA_PlaceGhost`
- `GA_Assemble`

**Available Blueprints** — click + and add in this order:
- `DA_Plot`
- `DA_WindTurbine`
- `DA_SolarPanel`
- `DA_Battery`
- `DA_BeeHive`

### 6. Placement Rules

All buildables are validated by `BuildManagerSubsystem`:
- Min 400 UU from hub center (safe zone clear)
- Min 200 UU from any existing station or ghost (no clipping)
- Max 2500 UU from hub center (play area)
- Soil-gated items (plots): radius scales with soil progress, can't build at 0%

### 7. Test the Build System

1. Press **B** to enter build mode — numbered list appears on left
2. Press **1-4** to select a buildable, or **B** to toggle
3. Aim at ground, press **LMB** to place
4. Free items (plots) spawn directly. Costed items spawn as blue ghost cubes.
5. Carry harvest crates to ghosts, **LMB** to deposit. Ghost turns green when funded.
6. Press **E** on funded ghost to assemble — real building spawns with animated parts
7. Wind turbine blades spin with wind, solar panel tracks the sun
8. Build a battery — watch HUD show charge level, amber text when discharging
9. Press **B** or **E** to exit build mode

---

## Bees (P4)

### 1. Create DA_BeeHive

In `Content/Kilnseed/Data/Blueprints/`:

Right-click > Data Asset > `BlueprintDataAsset`. Name it `DA_BeeHive`.

- **Blueprint Id:** `beehive`
- **Display Name:** `Bee Hive`
- **Actor Class:** `BeeHiveActor` (update to `BP_BeeHive` after step 2)
- **Resource Cost:** `2` (2 harvest crates to build)
- **Unlock Condition:** `unlock_bees` (must purchase "Unlock Bees" at console first)

### 2. Create BP_BeeHive

In `Content/Kilnseed/Blueprints/Stations/`:

Right-click > Blueprint Class > All Classes > search `BeeHiveActor` > name `BP_BeeHive`.

No mesh configuration needed — C++ builds the composite shape (amber house body + dark roof cone + entrance hole).

In Class Defaults (optional tweaks):
- **Max Bees:** `10` (bee slots per hive)
- **Crates Per Bee:** `1` (crates to build each additional bee)

Go back to `DA_BeeHive` and update **Actor Class** to `BP_BeeHive`.

### 3. How It Works

**Building the hive:**
1. Purchase "Unlock Bees" at the colony console (requires Atmo 25%)
2. Enter build mode, select Bee Hive (5), place it (costs 2 crates as ghost → fund → assemble)
3. Hive spawns with 1 bee and 10 slots

**Growing the colony:**
- Carry a harvest crate to the hive, **LMB** to deposit → builds a new bee (1 crate each)
- Prompt shows "Bees: X/10 slots | [LMB] Build Bee"
- Hive Expansion upgrades at the console add more slots

**Assigning bees:**
- Press **E** at the hive → assigns an idle bee to the most-needed role
- 5 roles: Pollinator, Hydrator, Harvester, Planter, Assembler
- Bees fly to their task, work, and return to the hive
- Each active bee draws 2W of power
- During brownout, bees return home and idle

### 4. Test Bees

1. Complete Atmo Tier I (deliver 5 aerolume to terraform hub)
2. At colony console, purchase "Unlock Bees" (2 crates)
3. Build a hive (enter build mode, key 5, place ghost, deliver 2 crates, assemble)
4. Hive appears with 1 bee — deliver more crates to the hive for more bees
5. Press **E** at hive to assign bees to tasks
6. Watch bees fly, work, and return with role-specific visual payloads

---

## Colony Console + Upgrades (P4)

### 1. Create BP_ColonyConsole

In `Content/Kilnseed/Blueprints/Stations/`:

Right-click > Blueprint Class > All Classes > search `ColonyConsoleActor` > name `BP_ColonyConsole`.

No configuration needed — C++ builds the desk + angled screen composite. Place inside the hub.

### 2. Create BP_UpgradeDeposit

Right-click > Blueprint Class > All Classes > search `UpgradeDepositActor` > name `BP_UpgradeDeposit`.

No configuration needed. Place next to the console inside the hub.

The deposit box auto-finds the console in the level.

### 3. Place in Level

Drag both into `L_MainPlanet`, inside the safe zone hub:
- `BP_ColonyConsole` — central position inside the hub
- `BP_UpgradeDeposit` — next to the console

### 4. How It Works

**Console** (E to interact):
- Press **E** to cycle through available upgrades
- Prompt shows: current upgrade name, cost, deposit progress

**Deposit Box** (LMB to deposit):
- Carry a harvest crate to the box, press **LMB** to deposit
- Deposits count toward the console's selected upgrade
- When fully funded, upgrade auto-applies

**Progression Tree:**

All upgrades are purchased with harvest crates at the deposit box. Crates spent here can't go to the terraform hub — invest in colony power vs. push terraform progress.

| # | Upgrade | Cost | Requires | Effect |
|---|---------|------|----------|--------|
| 1 | Bee Speed I | 2 | — | Bees 50% faster |
| 2 | Unlock Bees | 2 | Atmo 25% | Activate bee colony (3 bees) |
| 3 | Unlock Loamspine | 3 | Atmo 25% | Amber soil plant (1x) |
| 4 | Unlock Tidefern | 3 | Atmo 50% | Teal hydro plant (1x) |
| 5 | Extra Bees I | 3 | Atmo 50% | 2 more bees |
| 6 | Unlock Glowmoss | 4 | Soil 33% | Advanced atmo plant (3x) |
| 7 | Bee Speed II | 4 | Soil 33% | Bees 50% faster again |
| 8 | Unlock Crystalvine | 4 | Hydro 33% | Advanced soil plant (3x) |
| 9 | Extra Bees II | 5 | Hydro 33% | 3 more bees |
| 10 | Unlock Deepcoral | 5 | Atmo 100% + Soil 33% | Advanced hydro plant (3x) |
| 11 | Extra Bees III | 6 | Soil 100% + Hydro 33% | 4 more bees |

Locked upgrades appear grayed out with their unlock condition. Milestones fire automatically from terraform tier completions but only make upgrades *available for purchase*.

### Terraform Tier Recipes

Delivery is now recipe-based. Each axis has 3-4 tiers. Fulfill the recipe to advance.

**Atmosphere (4 tiers):**
| Tier | Gives | Recipe |
|------|-------|--------|
| I | 25% | 5x Aerolume |
| II | 50% | 10x Aerolume + 3x Loamspine |
| III | 75% | 8x Aerolume + 8x Glowmoss + 5x Loamspine |
| IV | 100% | 15x Glowmoss + 8x Crystalvine + 3x Deepcoral |

**Soil (3 tiers):**
| Tier | Gives | Recipe |
|------|-------|--------|
| I | 33% | 5x Loamspine |
| II | 66% | 10x Loamspine + 3x Tidefern |
| III | 100% | 8x Loamspine + 8x Crystalvine + 5x Deepcoral |

**Hydrosphere (3 tiers):**
| Tier | Gives | Recipe |
|------|-------|--------|
| I | 33% | 4x Tidefern |
| II | 66% | 8x Tidefern + 3x Aerolume |
| III | 100% | 6x Tidefern + 8x Deepcoral + 5x Glowmoss |

**Key design:** Higher tiers require cross-axis plants, forcing diversified production. T1 is achievable pre-prestige, T2 needs some colony investment, T3+ needs multiple prestige loops for efficiency. Advanced plants (3x value) are critical for managing the large quantities in later tiers.

### New Plant Data Assets

Create in `Content/Kilnseed/Data/Plants/`:

#### DA_Glowmoss
- Plant Id: `glowmoss` | Display Name: `Glowmoss`
- Plant Color: (0.7, 0.9, 0.2, 1) — yellow-green
- Growth Day Cycles: 1.0 | Water Drain: 0.033 | Pollination Window: 15
- Plant Tag: `Seed.Plant.Glowmoss`

#### DA_Crystalvine
- Plant Id: `crystalvine` | Display Name: `Crystalvine`
- Plant Color: (0.6, 0.3, 0.9, 1) — purple
- Growth Day Cycles: 1.5 | Water Drain: 0.04 | Pollination Window: 15
- Plant Tag: `Seed.Plant.Crystalvine`

#### DA_Deepcoral
- Plant Id: `deepcoral` | Display Name: `Deepcoral`
- Plant Color: (0.9, 0.3, 0.5, 1) — pink/magenta
- Growth Day Cycles: 2.0 | Water Drain: 0.05 | Pollination Window: 15
- Plant Tag: `Seed.Plant.Deepcoral`

Add all 6 plant data assets to `BP_SeedDispenser > Available Plants`.

---

## PCG Terraform Vegetation (P3 — optional)

### 1. Create the Graph

In `Content/Kilnseed/PCG/`: Right-click > PCG > PCG Graph > name `PCG_TerraformVegetation`. Double-click to open the PCG Graph Editor.

You'll see an **Input** node on the left and an **Output** node on the right. Build the chain below between them. Right-click empty space to add each node.

### 2. Node-by-Node Setup

Build this graph left-to-right. Connect the **Out** pin of each node to the **In** pin of the next unless noted otherwise.

---

**Node 1: Surface Sampler**

Right-click > Generate > Surface Sampler.

| Property | Value |
|----------|-------|
| Points Per Square Meter | `0.02` |
| Point Extents | `(50, 50, 50)` |
| Unbounded | `false` |

This scatters sparse points across any landscape/surface under the actor.

Connect: **Input** → **Surface Sampler**

---

**Node 2: Density Noise**

Right-click > Point Operations > Density Noise.

| Property | Value |
|----------|-------|
| Noise Type | Perlin |
| Cell Size | `500` |
| Density Function | Set (not Multiply) |

This gives each point a random density between 0 and 1, creating natural variation — some areas dense, some sparse.

Connect: **Surface Sampler** → **Density Noise**

---

**Node 3: Get Actor Property**

Right-click > Properties > Get Actor Property.

| Property | Value |
|----------|-------|
| Actor Selection | Self |
| Property Name | `SoilProgress` |
| Output Attribute Name | `SoilProgress` |

This reads the `SoilProgress` float from the TerraformPCGActor and stores it as a per-point attribute.

Connect: **Density Noise** → **Get Actor Property**

---

**Node 4: Attribute Operation (Scale Density by Soil)**

Right-click > Point Operations > Attribute Operation.

| Property | Value |
|----------|-------|
| Operation | Multiply |
| Input Source 1 > Selection | Attribute — `$Density` |
| Input Source 2 > Selection | Attribute — `SoilProgress` |
| Output Target > Selection | Attribute — `$Density` |

This multiplies each point's density by SoilProgress. At 0% soil, all densities become 0 (nothing spawns). At 100%, densities are unchanged.

Connect: **Get Actor Property** → **Attribute Operation**

---

**Node 5: Density Filter (Cull Empty Points)**

Right-click > Filter > Density Filter.

| Property | Value |
|----------|-------|
| Lower Bound | `0.1` |
| Upper Bound | `1.0` |

Removes all points with density below 0.1 (the near-zero ones that would produce nothing).

Connect: **Attribute Operation** → **Density Filter**

The **Inside** pin continues the chain. The **Outside** pin is unused (culled points).

---

**Node 6: Bounds Modifier (Hub Exclusion)**

Right-click > Spatial > Bounds Modifier.

| Property | Value |
|----------|-------|
| Mode | Intersect |
| Bounds Min | `(-2500, -2500, -5000)` |
| Bounds Max | `(2500, 2500, 5000)` |
| Invert | `false` |

Then add a **second Bounds Modifier** to cut out the center:

| Property | Value |
|----------|-------|
| Mode | Difference |
| Bounds Min | `(-800, -800, -5000)` |
| Bounds Max | `(800, 800, 5000)` |

Alternative approach: use a **Distance** node (Spatial > Distance, Origin = (0,0,0)) followed by a **Point Filter** (Filter > Point Filter, Attribute = `$Distance`, Operator = GreaterThan, Threshold = 800). This removes points within 800 UU of the hub.

Connect: **Density Filter (Inside)** → **Bounds Modifier(s)**

---

**Node 7: Copy Points (Split into 3 branches)**

From the last node's output, drag three separate connections. Each branch will filter for a different vegetation tier.

**--- Branch A: Grass (all surviving points) ---**

**Node 8A: Static Mesh Spawner (Grass)**

Right-click > Output > Static Mesh Spawner.

| Property | Value |
|----------|-------|
| Mesh Entries > + Add | |
| Mesh | `/Engine/BasicShapes/Sphere.Sphere` |
| Scale Min | `(0.05, 0.05, 0.03)` |
| Scale Max | `(0.08, 0.08, 0.05)` |
| Material Override | MI_Grass (optional, see step 4) |

Connect: **Bounds output** → **Static Mesh Spawner (Grass)** → **Output**

**--- Branch B: Shrubs (dense areas only) ---**

**Node 8B: Density Remap**

Right-click > Point Operations > Density Remap.

| Property | Value |
|----------|-------|
| In Range Min | `0.5` |
| In Range Max | `1.0` |
| Out Range Min | `0.0` |
| Out Range Max | `1.0` |

Points with original density < 0.5 get density 0. Points 0.5–1.0 get remapped to 0–1.

**Node 9B: Density Filter**

| Property | Value |
|----------|-------|
| Lower Bound | `0.1` |
| Upper Bound | `1.0` |

Removes the zeroed-out points.

**Node 10B: Static Mesh Spawner (Shrubs)**

| Property | Value |
|----------|-------|
| Mesh | `/Engine/BasicShapes/Sphere.Sphere` |
| Scale Min | `(0.12, 0.12, 0.15)` |
| Scale Max | `(0.18, 0.18, 0.22)` |
| Material Override | MI_Shrub (optional) |

Connect: **Bounds output** → **Density Remap** → **Density Filter** → **Mesh Spawner (Shrubs)** → **Output**

**--- Branch C: Trees (very dense areas only) ---**

**Node 8C: Density Remap**

| Property | Value |
|----------|-------|
| In Range Min | `0.8` |
| In Range Max | `1.0` |
| Out Range Min | `0.0` |
| Out Range Max | `1.0` |

**Node 9C: Density Filter**

| Property | Value |
|----------|-------|
| Lower Bound | `0.1` |
| Upper Bound | `1.0` |

**Node 10C: Static Mesh Spawner (Tree Trunk)**

| Property | Value |
|----------|-------|
| Mesh | `/Engine/BasicShapes/Cylinder.Cylinder` |
| Scale Min / Max | `(0.04, 0.04, 0.3)` |
| Material Override | MI_TreeTrunk (optional) |

**Node 11C: Static Mesh Spawner (Tree Canopy)**

| Property | Value |
|----------|-------|
| Mesh | `/Engine/BasicShapes/Sphere.Sphere` |
| Scale Min / Max | `(0.2, 0.2, 0.15)` |
| Offset | `(0, 0, 60)` |
| Material Override | MI_TreeCanopy (optional) |

Connect: **Bounds output** → **Density Remap** → **Density Filter** → **Mesh Spawner (Trunk)** → **Output**

Connect separately: **Density Filter (same one)** → **Mesh Spawner (Canopy)** → **Output**

(The canopy spawner reads from the same filtered points as the trunk but places meshes higher via the offset.)

---

### 3. Optional: PCG_TerraformWater

Create a second graph `PCG_TerraformWater` using the same pattern but:

| Difference | Value |
|------------|-------|
| Surface Sampler density | `0.005` (very sparse) |
| Actor Property | `HydroProgress` instead of `SoilProgress` |
| After density filter, add a **Point Filter** | Attribute: `$Position.Z`, Operator: LessThan, Threshold: `50` (only low terrain) |
| Single mesh spawner | Mesh: Cylinder, Scale: random `(0.5-1.5, 0.5-1.5, 0.01)` — flat discs |
| Material | MI_Water (blue emissive) |

### 4. Place TerraformPCGActor

1. Place Actors > All Classes > search `TerraformPCGActor` > drag into level at **(0, 0, 0)**
2. In Details panel, find **PCG Component**
3. Set **Graph**: `PCG_TerraformVegetation`
4. Set **Generation Trigger**: `Generate On Demand`

The C++ actor ticks every 0.5s, reads SoilProgress/HydroProgress from TerraformManagerSubsystem, and calls Cleanup()+Generate() when progress changes by more than 5%.

For water: add a second TerraformPCGActor, assign `PCG_TerraformWater`.

### 5. Materials (optional but recommended)

Create Material Instances in `Content/Kilnseed/Materials/`. For each, set the emissive color:

| Asset | Base Color |
|-------|-----------|
| MI_Grass | `(0.3, 0.8, 0.2)` green |
| MI_Shrub | `(0.2, 0.6, 0.15)` dark green |
| MI_TreeCanopy | `(0.15, 0.5, 0.1)` deep green |
| MI_TreeTrunk | `(0.4, 0.25, 0.1)` brown |
| MI_Water | `(0.2, 0.4, 0.9)` blue |

Assign in each Static Mesh Spawner's **Material Override** slot. Without these, meshes use default grey.

---

## Quick Reference — All Editor Assets

### P1-P2 (already created)

```
Content/Kilnseed/
  Input/
    IA_Move, IA_Look, IA_Jump, IA_Sprint,
    IA_Interact, IA_PrimaryAction, IA_BuildMenu,
    IA_Flashlight, IMC_Default
  Data/
    Effects/  GE_O2Drain, GE_O2Refill, GE_PlotGrowth,
              GE_WaterDrain, GE_SprintO2Drain
    Plants/   DA_Aerolume, DA_Loamspine, DA_Tidefern
  Blueprints/
    BP_KilnseedGameMode
    Player/   BP_KilnseedPlayer
    Stations/ BP_SafeZone, BP_Plot, BP_SeedDispenser,
              BP_WaterReservoir, BP_TerraformHub
  Maps/
    L_MainPlanet
```

### P3 (build system + power)

```
Content/Kilnseed/
  Input/
    IA_BuildSelect
  Data/
    Blueprints/  DA_Plot, DA_WindTurbine, DA_SolarPanel, DA_Battery
  Blueprints/
    Stations/    BP_WindTurbine, BP_SolarPanel, BP_Battery
  PCG/
    PCG_TerraformVegetation (optional)
    PCG_TerraformWater (optional)
```

### P4 (bees + upgrades)

```
Content/Kilnseed/
  Blueprints/
    Stations/    BP_BeeHive, BP_ColonyConsole, BP_UpgradeDeposit
```
