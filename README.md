# Kilnseed

Multiplayer co-op sci-fi terraforming/farming game built in Unreal Engine 5.7. Land on a barren alien planet, farm exotic crops, automate with bee drones, and terraform it into a livable world.

## Core Loop

Plant → Grow → Water → Pollinate → Harvest → **Deliver** (terraform progress) or **Spend** (build structures/upgrades)

| Plant | Growth | Terraform Axis | Color |
|-------|--------|---------------|-------|
| Aerolume | ~25s | Atmosphere | Lime Green |
| Loamspine | ~50s | Soil | Amber |
| Tidefern | ~100s | Hydrosphere | Teal |

Win condition: all 3 axes at 100%. Delivery quotas scale by player count.

## Multiplayer

- Up to 4 players, online co-op + split-screen
- Server-authoritative replication
- Per-player O2 survival (ASC on PlayerState, survives respawn)
- Shared resources, shared world progression

## UE5.7 Systems

| System | Usage |
|--------|-------|
| GAS | Player attributes (O2), plot attributes (growth/water), abilities (interact/pickup/place/harvest), periodic effects (drain/refill/growth), gameplay cues |
| PCG | Procedural rock/mountain scattering, milestone-driven vegetation |
| StateTree + Smart Objects | Bee AI task selection and execution |
| Common UI | Per-player widget stacks for split-screen |
| Enhanced Input | Per-player input mapping contexts |
| Niagara | Dust, bee trails, pollen, rain, bloom FX |

## Project Structure

```
Source/Kilnseed/
  Core/           10 subsystems (EventBus, TickEngine, PlotManager, PowerManager, etc.)
  GAS/            ASC, 3 attribute sets, 5 abilities
  Multiplayer/    GameMode, GameState, PlayerState (replication framework)
  Player/         Character, InteractionComponent, CarryComponent
  Stations/       Plot, SeedDispenser, WaterReservoir, TerraformHub, SolarPanel, SafeZone
  Items/          CarriableBase (seed pods, canisters, crates)
  Bees/           BeeActor, BeeAIController (stubs for StateTree)
  Build/          BuildGhostActor
  Data/           PlantDataAsset, MilestoneDataAsset, BeeUpgradeDataAsset, BlueprintDataAsset
  Save/           KilnseedSaveGame
```

## Setup

1. Requires Unreal Engine 5.7
2. Clone the repo
3. Right-click `Kilnseed.uproject` → Generate Visual Studio project files
4. Build from Visual Studio
5. Follow `EDITOR_SETUP.md` to create required editor assets (input, gameplay effects, blueprints, level)

## Implementation Status

| Phase | Status | Description |
|-------|--------|-------------|
| P0 | C++ done | Scaffold, GAS, tags, subsystems, multiplayer framework |
| P1 | C++ done, needs editor assets | Walking, O2 survival, safe zone, death/respawn |
| P2 | C++ done, needs editor assets | Farming loop (plant, grow, water, pollinate, harvest) |
| P3 | C++ done, needs editor assets | Delivery, power economy, build system, milestones |
| P4 | Not started | Bee AI (StateTree + Smart Objects) |
| P5 | Not started | Bee upgrades, all 5 roles |
| P6 | Not started | World progression (sky/fog/PCG vegetation), win condition |
| P7 | Not started | Polish, audio, VFX, balance |

## Origin

Adapted from Worldseed, a Godot 4.6 prototype with the same design. Kilnseed ports the core loop to UE5.7 with multiplayer, GAS, and greybox art.