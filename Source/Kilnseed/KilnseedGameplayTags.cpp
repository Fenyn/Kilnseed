#include "KilnseedGameplayTags.h"

namespace KilnseedTags
{
	UE_DEFINE_GAMEPLAY_TAG(Plant_Aerolume,    "Seed.Plant.Aerolume");
	UE_DEFINE_GAMEPLAY_TAG(Plant_Loamspine,   "Seed.Plant.Loamspine");
	UE_DEFINE_GAMEPLAY_TAG(Plant_Tidefern,    "Seed.Plant.Tidefern");
	UE_DEFINE_GAMEPLAY_TAG(Plant_Glowmoss,    "Seed.Plant.Glowmoss");
	UE_DEFINE_GAMEPLAY_TAG(Plant_Crystalvine, "Seed.Plant.Crystalvine");
	UE_DEFINE_GAMEPLAY_TAG(Plant_Deepcoral,   "Seed.Plant.Deepcoral");

	UE_DEFINE_GAMEPLAY_TAG(Plot_Empty,        "Seed.Plot.Empty");
	UE_DEFINE_GAMEPLAY_TAG(Plot_Planted,      "Seed.Plot.Planted");
	UE_DEFINE_GAMEPLAY_TAG(Plot_Growing,      "Seed.Plot.Growing");
	UE_DEFINE_GAMEPLAY_TAG(Plot_Pollinating,  "Seed.Plot.Pollinating");
	UE_DEFINE_GAMEPLAY_TAG(Plot_Bloomed,      "Seed.Plot.Bloomed");

	UE_DEFINE_GAMEPLAY_TAG(Bee_Role_Idle,       "Seed.Bee.Role.Idle");
	UE_DEFINE_GAMEPLAY_TAG(Bee_Role_Pollinator, "Seed.Bee.Role.Pollinator");
	UE_DEFINE_GAMEPLAY_TAG(Bee_Role_Hydrator,   "Seed.Bee.Role.Hydrator");
	UE_DEFINE_GAMEPLAY_TAG(Bee_Role_Harvester,  "Seed.Bee.Role.Harvester");
	UE_DEFINE_GAMEPLAY_TAG(Bee_Role_Planter,    "Seed.Bee.Role.Planter");
	UE_DEFINE_GAMEPLAY_TAG(Bee_Role_Assembler,  "Seed.Bee.Role.Assembler");

	UE_DEFINE_GAMEPLAY_TAG(State_InSafeZone,  "Seed.State.InSafeZone");
	UE_DEFINE_GAMEPLAY_TAG(State_Brownout,    "Seed.State.Brownout");
	UE_DEFINE_GAMEPLAY_TAG(State_Carrying,    "Seed.State.Carrying");
	UE_DEFINE_GAMEPLAY_TAG(State_BuildMode,   "Seed.State.BuildMode");
	UE_DEFINE_GAMEPLAY_TAG(State_Dead,        "Seed.State.Dead");

	UE_DEFINE_GAMEPLAY_TAG(Milestone_Atmo25,   "Seed.Milestone.Atmo25");
	UE_DEFINE_GAMEPLAY_TAG(Milestone_Atmo50,   "Seed.Milestone.Atmo50");
	UE_DEFINE_GAMEPLAY_TAG(Milestone_Atmo100,  "Seed.Milestone.Atmo100");
	UE_DEFINE_GAMEPLAY_TAG(Milestone_Soil33,   "Seed.Milestone.Soil33");
	UE_DEFINE_GAMEPLAY_TAG(Milestone_Soil66,   "Seed.Milestone.Soil66");
	UE_DEFINE_GAMEPLAY_TAG(Milestone_Soil100,  "Seed.Milestone.Soil100");
	UE_DEFINE_GAMEPLAY_TAG(Milestone_Hydro33,  "Seed.Milestone.Hydro33");
	UE_DEFINE_GAMEPLAY_TAG(Milestone_Hydro66,  "Seed.Milestone.Hydro66");
	UE_DEFINE_GAMEPLAY_TAG(Milestone_Hydro100, "Seed.Milestone.Hydro100");

	UE_DEFINE_GAMEPLAY_TAG(Ability_Interact,   "Seed.Ability.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Pickup,     "Seed.Ability.Pickup");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Place,      "Seed.Ability.Place");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Harvest,    "Seed.Ability.Harvest");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Pollinate,  "Seed.Ability.Pollinate");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Build,      "Seed.Ability.Build");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Assemble,   "Seed.Ability.Assemble");

	UE_DEFINE_GAMEPLAY_TAG(SmartObject_NeedsPollination, "Seed.SmartObject.NeedsPollination");
	UE_DEFINE_GAMEPLAY_TAG(SmartObject_NeedsWater,       "Seed.SmartObject.NeedsWater");
	UE_DEFINE_GAMEPLAY_TAG(SmartObject_ReadyToHarvest,   "Seed.SmartObject.ReadyToHarvest");
	UE_DEFINE_GAMEPLAY_TAG(SmartObject_NeedsPlanting,    "Seed.SmartObject.NeedsPlanting");
	UE_DEFINE_GAMEPLAY_TAG(SmartObject_NeedsAssembly,    "Seed.SmartObject.NeedsAssembly");
	UE_DEFINE_GAMEPLAY_TAG(SmartObject_SeedSource,       "Seed.SmartObject.SeedSource");
	UE_DEFINE_GAMEPLAY_TAG(SmartObject_WaterSource,      "Seed.SmartObject.WaterSource");
	UE_DEFINE_GAMEPLAY_TAG(SmartObject_DeliveryTarget,   "Seed.SmartObject.DeliveryTarget");

	UE_DEFINE_GAMEPLAY_TAG(Item_Seed,            "Seed.Item.Seed");
	UE_DEFINE_GAMEPLAY_TAG(Item_WaterCanister,   "Seed.Item.WaterCanister");
	UE_DEFINE_GAMEPLAY_TAG(Item_HarvestCrate,    "Seed.Item.HarvestCrate");

	UE_DEFINE_GAMEPLAY_TAG(Data_GrowthRate,   "Seed.Data.GrowthRate");
	UE_DEFINE_GAMEPLAY_TAG(Data_WaterDrain,   "Seed.Data.WaterDrain");

	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Harvest,   "GameplayCue.Seed.Harvest");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Deliver,   "GameplayCue.Seed.Deliver");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Milestone, "GameplayCue.Seed.Milestone");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Pollinate, "GameplayCue.Seed.Pollinate");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Brownout,  "GameplayCue.Seed.Brownout");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_O2Warning, "GameplayCue.Seed.O2Warning");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Death,     "GameplayCue.Seed.Death");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Build,     "GameplayCue.Seed.Build");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_SafeZone,  "GameplayCue.Seed.SafeZone");
}
