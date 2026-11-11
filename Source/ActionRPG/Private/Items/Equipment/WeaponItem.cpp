// Copyright Epic Games, Inc. All Rights Reserved.

#include "Items/Equipment/WeaponItem.h"
#include "Items/Core/ItemDataAsset.h"

float UWeaponItem::GetWeaponDamage() const
{
	if (!ItemData) return 0.0f;
	const float Base = FMath::IsFinite(ItemData->WeaponDamage) ? FMath::Max(0.0f, ItemData->WeaponDamage) : 0.0f;
	if (!ItemData->bSoulboundGrowth || !FMath::IsFinite(ItemData->DamagePerGrowthRank)) return Base;
	return Base + GrowthRank * FMath::Max(0.0f, ItemData->DamagePerGrowthRank);
}

FText UWeaponItem::GetItemTooltip() const
{
	const FString Base = Super::GetItemTooltip().ToString();
	if (!ItemData || !ItemData->bSoulboundGrowth) return FText::FromString(Base);
	return FText::FromString(FString::Printf(TEXT("%s\nSoulbound rank: %d / %d\nDefeats toward next rank: %d / %d\nCurrent weapon bonus: +%.0f"),
		*Base, GrowthRank, FMath::Max(1, ItemData->MaxGrowthRank), DefeatsTowardGrowth,
		FMath::Max(1, ItemData->DefeatsPerGrowthRank), GetWeaponDamage()));
}

bool UWeaponItem::BindToOwner(AActor* NewOwner)
{
	if (!ItemData || !ItemData->bSoulboundGrowth) return true;
	if (!IsValid(NewOwner) || IsBoundToOtherOwner(NewOwner)) return false;
	if (!BoundOwner) BoundOwner = NewOwner;
	return true;
}

bool UWeaponItem::IsBoundToOtherOwner(const AActor* NewOwner) const
{
	return ItemData && ItemData->bSoulboundGrowth && BoundOwner && BoundOwner != NewOwner;
}

bool UWeaponItem::RecordCreditedDefeat()
{
	if (!ItemData || !ItemData->bSoulboundGrowth || !BoundOwner
		|| GrowthRank >= FMath::Max(1, ItemData->MaxGrowthRank)) return false;
	++DefeatsTowardGrowth;
	if (DefeatsTowardGrowth < FMath::Max(1, ItemData->DefeatsPerGrowthRank)) return false;
	DefeatsTowardGrowth = 0;
	++GrowthRank;
	return true;
}

EDamageType UWeaponItem::GetWeaponDamageType() const
{
	return ItemData ? ItemData->WeaponDamageType : EDamageType::Physical;
}
