// Copyright Epic Games, Inc. All Rights Reserved.

#include "Items/Core/ItemDataAsset.h"

UItemDataAsset::UItemDataAsset()
{
	// Set default values
	ItemType = FPrimaryAssetType("Item");
	ItemID = NAME_None;
	ItemName = FText::GetEmpty();
	ItemDescription = FText::GetEmpty();
	ItemIcon = nullptr;
	ItemPickupActorClass = nullptr;
	ItemClass = nullptr;
	Type = EItemType::Misc;
	Rarity = EItemRarity::Common;
	MaxStackSize = 1;
	Weight = 0.0f;
	Value = 0;
	ConsumableRestoreAmount = 25.0f;
	EquipmentSlot = EEquipmentSlot::None;
	EquipmentBonuses = {};
	EquipmentPrimaryBonuses = {};
	EquipmentRequiredLevel = 1;
	EquipmentRequiredAttributes = {};
	WeaponDamage = 0.0f;
	WeaponDamageType = EDamageType::Physical;
	SkillData = nullptr;
	SkillClass = nullptr;
	RequiredLevel = 1;
	RequiredAttributes = {};
}

FPrimaryAssetId UItemDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(ItemType, ItemID);
}

FText UItemDataAsset::GetItemTooltip() const
{
	TArray<FString> Lines;
	Lines.Add(ItemName.ToString());
	if (!ItemDescription.IsEmpty()) Lines.Add(ItemDescription.ToString());
	if (Type == EItemType::Equipment)
	{
		if (EquipmentRequiredLevel > 1)
		{
			Lines.Add(FString::Printf(TEXT("Requires player level %d"), EquipmentRequiredLevel));
		}
		const UEnum* Attributes = StaticEnum<EPrimaryAttribute>();
		TArray<FString> Requirements;
		for (const TPair<EPrimaryAttribute, float>& Pair : EquipmentRequiredAttributes)
		{
			const FString Name = Attributes
				? Attributes->GetDisplayNameTextByValue(static_cast<int64>(Pair.Key)).ToString()
				: TEXT("Attribute");
			Requirements.Add(FString::Printf(TEXT("Requires %s %.0f"), *Name, Pair.Value));
		}
		Requirements.Sort();
		Lines.Append(Requirements);
		TArray<FString> Bonuses;
		for (const TPair<EPrimaryAttribute, float>& Pair : EquipmentPrimaryBonuses)
		{
			if (!FMath::IsFinite(Pair.Value) || FMath::IsNearlyZero(Pair.Value)) continue;
			const FString Name = Attributes
				? Attributes->GetDisplayNameTextByValue(static_cast<int64>(Pair.Key)).ToString()
				: TEXT("Attribute");
			Bonuses.Add(FString::Printf(TEXT("%s: %+.0f"), *Name, Pair.Value));
		}
		const UEnum* SecondaryAttributes = StaticEnum<ESecondaryAttribute>();
		for (const TPair<ESecondaryAttribute, float>& Pair : EquipmentBonuses)
		{
			if (!FMath::IsFinite(Pair.Value) || FMath::IsNearlyZero(Pair.Value)) continue;
			const FString Name = SecondaryAttributes
				? SecondaryAttributes->GetDisplayNameTextByValue(static_cast<int64>(Pair.Key)).ToString()
				: TEXT("Stat");
			Bonuses.Add(FString::Printf(TEXT("%s: %+.0f"), *Name, Pair.Value));
		}
		Bonuses.Sort();
		Lines.Append(Bonuses);
		const UEnum* DamageTypes = StaticEnum<EDamageType>();
		if (FMath::IsFinite(WeaponDamage) && WeaponDamage > 0.0f)
		{
			const FString TypeName = DamageTypes
				? DamageTypes->GetDisplayNameTextByValue(static_cast<int64>(WeaponDamageType)).ToString()
				: TEXT("Physical");
			Lines.Add(FString::Printf(TEXT("Weapon Bonus: +%.0f %s"), WeaponDamage, *TypeName));
		}
		TArray<FString> Resistances;
		for (const TPair<EDamageType, float>& Pair : EquipmentDamageResistanceBonuses)
		{
			if (!FMath::IsFinite(Pair.Value) || FMath::IsNearlyZero(Pair.Value)) continue;
			const FString Name = DamageTypes
				? DamageTypes->GetDisplayNameTextByValue(static_cast<int64>(Pair.Key)).ToString()
				: TEXT("Damage");
			Resistances.Add(FString::Printf(TEXT("%s Resistance: %+.0f%%"), *Name, Pair.Value));
		}
		Resistances.Sort();
		Lines.Append(Resistances);
	}
	return FText::FromString(FString::Join(Lines, TEXT("\n")));
}

