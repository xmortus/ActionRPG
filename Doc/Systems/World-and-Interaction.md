# World, movement, interaction, and data lookup

## Player and camera

`AActionRPGGameMode` sets the native player character, controller, game state, and HUD defaults; project Blueprints can provide configured subclasses. `AActionRPGPlayerCharacter` creates the top-down spring arm/camera, its gameplay components, and a separate visual mesh for an equipped main-hand weapon. Character movement does not automatically rotate the pawn toward movement. The player controller handles movement and look input, and the player character uses its tick for cursor-facing behavior. Dodge is a separate stamina-consuming movement action described in [Combat and Resources](Combat-and-Resources.md).

The equipped weapon visual follows `UEquipmentComponent` events; it is presentation, while weapon damage/type come from item data. Source: [game mode](../../Source/ActionRPG/Private/Core/ActionRPGGameMode.cpp), [player character](../../Source/ActionRPG/Private/Characters/ActionRPGPlayerCharacter.cpp), [controller](../../Source/ActionRPG/Private/Core/ActionRPGPlayerController.cpp).

## World interaction

The controller's Interact action searches for a nearby pickup in the cursor-directed cone and checks the pickup's interaction range. `AItemPickupActor` may also auto-pick up on overlap when configured. A successful pickup creates an item through `UItemDatabase`, asks `UInventoryComponent` to add the full quantity, plays its pickup effect, and destroys the world actor. A capacity failure keeps the world actor. Trial markers use `ATrialTriggerActor` to start or advance trials. Their in-world text includes the saved trial name, so the prototype route can distinguish its start and goal from the other markers. Source: [pickup](../../Source/ActionRPG/Private/Items/Pickups/ItemPickupActor.cpp), [trial trigger](../../Source/ActionRPG/Private/Trials/TrialTriggerActor.cpp).

## Asset registries

`UItemDatabase`, `USkillDatabase`, and `UClassDatabase` are lookup helpers for their data assets. Item lookup uses the Unreal Asset Manager's primary asset list and item IDs. Primary asset scan settings live in [DefaultGame.ini](../../Config/DefaultGame.ini); a missing item in play should be checked against its asset type, ID, and scan path before changing inventory logic. The `/Game/Data` inventory is listed in [the index](README.md). Source: [item database](../../Source/ActionRPG/Private/Data/ItemDatabase.cpp), [skill database](../../Source/ActionRPG/Private/Data/SkillDatabase.cpp), [class database](../../Source/ActionRPG/Private/Data/ClassDatabase.cpp).
