// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Equipment/EquipmentStatsWidget.h"
#include "UI/ActionRPGUITheme.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Inventory/EquipmentComponent.h"
#include "Components/Progression/AttributeComponent.h"
#include "Components/Progression/SecondaryAttributeComponent.h"
#include "Components/Skills/SkillManagerComponent.h"
#include "Components/Skills/SkillComponent.h"
#include "Components/TextBlock.h"
#include "GameFramework/Pawn.h"
#include "Items/Core/ItemDataAsset.h"
#include "Items/Equipment/EquipmentItem.h"
#include "Items/Equipment/WeaponItem.h"
#include "Skills/Core/SkillDataAsset.h"
#include "Skills/Implementations/SkillMeleeAttack.h"
#include "Skills/Implementations/SkillRangedAttack.h"
#include "Skills/Implementations/SkillMagic.h"

void UEquipmentStatsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ActionRPGUITheme::StyleBlueprintWidgetTree(WidgetTree);
	if (WidgetTree)
	{
		if (!PrimaryStatsText)
		{
			PrimaryStatsText = Cast<UTextBlock>(WidgetTree->FindWidget(TEXT("EPrimaryStats")));
		}
		if (!SecondaryStatsText)
		{
			SecondaryStatsText = Cast<UTextBlock>(WidgetTree->FindWidget(TEXT("ESecondaryStats")));
		}
		TArray<UWidget*> Widgets;
		WidgetTree->GetAllWidgets(Widgets);
		for (UWidget* Widget : Widgets)
		{
			if (UTextBlock* StatText = Cast<UTextBlock>(Widget))
			{
				FSlateFontInfo Font = StatText->GetFont();
				Font.Size = 14;
				StatText->SetFont(Font);
			}
		}
	}
	if (PrimaryStatsText)
	{
		PrimaryStatsText->RemoveFromParent();
	}
	if (SecondaryStatsText)
	{
		SecondaryStatsText->SetAutoWrapText(true);
		if (UCanvasPanelSlot* TextSlot = Cast<UCanvasPanelSlot>(SecondaryStatsText->Slot))
		{
			TextSlot->SetAnchors(FAnchors(0.0f, 0.0f));
			TextSlot->SetAlignment(FVector2D::ZeroVector);
			TextSlot->SetAutoSize(false);
			TextSlot->SetPosition(FVector2D::ZeroVector);
			TextSlot->SetSize(FVector2D(470.0f, 340.0f));
		}
	}

	BindComponents();
	RefreshStats();
}

void UEquipmentStatsWidget::NativeDestruct()
{
	if (AttributeComponent)
	{
		AttributeComponent->OnPrimaryAttributeChanged.RemoveDynamic(this, &UEquipmentStatsWidget::HandlePrimaryAttributeChanged);
	}

	if (SecondaryAttributeComponent)
	{
		SecondaryAttributeComponent->OnSecondaryAttributeChanged.RemoveDynamic(this, &UEquipmentStatsWidget::HandleSecondaryAttributeChanged);
		SecondaryAttributeComponent->OnCurrentSecondaryAttributeChanged.RemoveDynamic(this, &UEquipmentStatsWidget::HandleCurrentSecondaryAttributeChanged);
	}

	if (SkillManagerComponent)
	{
		SkillManagerComponent->OnMainHandSkillChanged.RemoveDynamic(this, &UEquipmentStatsWidget::HandleMainHandSkillChanged);
	}
	if (SkillComponent)
	{
		SkillComponent->OnSkillLevelUp.RemoveDynamic(this, &UEquipmentStatsWidget::HandleSkillLevelUp);
	}

	Super::NativeDestruct();
}

void UEquipmentStatsWidget::BindComponents()
{
	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!OwningPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentStatsWidget - No owning pawn found."));
		return;
	}

	AttributeComponent = OwningPawn->FindComponentByClass<UAttributeComponent>();
	SecondaryAttributeComponent = OwningPawn->FindComponentByClass<USecondaryAttributeComponent>();
	SkillManagerComponent = OwningPawn->FindComponentByClass<USkillManagerComponent>();
	SkillComponent = OwningPawn->FindComponentByClass<USkillComponent>();

	if (AttributeComponent)
	{
		AttributeComponent->OnPrimaryAttributeChanged.AddUniqueDynamic(this, &UEquipmentStatsWidget::HandlePrimaryAttributeChanged);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentStatsWidget - AttributeComponent not found."));
	}

	if (SecondaryAttributeComponent)
	{
		SecondaryAttributeComponent->OnSecondaryAttributeChanged.AddUniqueDynamic(this, &UEquipmentStatsWidget::HandleSecondaryAttributeChanged);
		SecondaryAttributeComponent->OnCurrentSecondaryAttributeChanged.AddUniqueDynamic(this, &UEquipmentStatsWidget::HandleCurrentSecondaryAttributeChanged);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentStatsWidget - SecondaryAttributeComponent not found."));
	}

	if (SkillManagerComponent)
	{
		SkillManagerComponent->OnMainHandSkillChanged.AddUniqueDynamic(this, &UEquipmentStatsWidget::HandleMainHandSkillChanged);
	}
	if (SkillComponent)
	{
		SkillComponent->OnSkillLevelUp.AddUniqueDynamic(this, &UEquipmentStatsWidget::HandleSkillLevelUp);
	}
}

void UEquipmentStatsWidget::HandlePrimaryAttributeChanged(EPrimaryAttribute Attribute, float NewValue, float OldValue)
{
	RefreshStats();
}

void UEquipmentStatsWidget::HandleSecondaryAttributeChanged(ESecondaryAttribute Attribute, float NewValue)
{
	RefreshStats();
}

void UEquipmentStatsWidget::HandleCurrentSecondaryAttributeChanged(ESecondaryAttribute Attribute, float NewValue, float OldValue)
{
	RefreshStats();
}

void UEquipmentStatsWidget::HandleMainHandSkillChanged(USkillBase* Skill)
{
	RefreshStats();
}

void UEquipmentStatsWidget::HandleSkillLevelUp(USkillBase* Skill, int32 NewLevel)
{
	if (SkillManagerComponent && SkillManagerComponent->GetMainHandSkill() == Skill)
	{
		RefreshStats();
	}
}

void UEquipmentStatsWidget::RefreshStats()
{
	OnStatsUpdated();
	if (SecondaryStatsText)
	{
		SecondaryStatsText->SetText(BuildSecondaryAttributeText());
	}
}

void UEquipmentStatsWidget::OnStatsUpdated_Implementation()
{
	if (PrimaryStatsText)
	{
		PrimaryStatsText->SetText(FText::GetEmpty());
		PrimaryStatsText->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (SecondaryStatsText)
	{
		SecondaryStatsText->SetText(GetSecondaryAttributesText());
	}
}

FText UEquipmentStatsWidget::GetPrimaryAttributesText() const
{
	return BuildPrimaryAttributeText();
}

FText UEquipmentStatsWidget::GetSecondaryAttributesText() const
{
	return BuildSecondaryAttributeText();
}

FText UEquipmentStatsWidget::BuildPrimaryAttributeText() const
{
	if (!AttributeComponent)
	{
		return FText::FromString(TEXT("None"));
	}

	const UEnum* Enum = StaticEnum<EPrimaryAttribute>();
	TArray<FString> Lines;
	const int32 EnumCount = Enum ? Enum->NumEnums() : 0;
	for (int32 Index = 0; Index < EnumCount; ++Index)
	{
		if (Enum)
		{
			const FString EnumName = Enum->GetNameStringByIndex(Index);
			if (EnumName.EndsWith(TEXT("MAX")))
			{
				continue;
			}
		}

		const EPrimaryAttribute Attribute = Enum ? static_cast<EPrimaryAttribute>(Enum->GetValueByIndex(Index)) : EPrimaryAttribute::Strength;
		const float Value = AttributeComponent->GetAttribute(Attribute);
		const FString DisplayName = Enum ? Enum->GetDisplayNameTextByIndex(Index).ToString() : TEXT("Attribute");
		Lines.Add(FString::Printf(TEXT("%s: %.0f"), *DisplayName, Value));
	}

	if (Lines.Num() == 0)
	{
		return FText::FromString(TEXT("None"));
	}

	Lines.Add(FString::Printf(TEXT("Free Attribute Points: %d"),
		AttributeComponent ? AttributeComponent->GetUnspentAttributePoints() : 0));

	return FText::FromString(FString::Join(Lines, TEXT("\n")));
}

FText UEquipmentStatsWidget::BuildSecondaryAttributeText() const
{
	if (!SecondaryAttributeComponent)
	{
		return FText::FromString(TEXT("Stats unavailable"));
	}

	const USecondaryAttributeComponent* Stats = SecondaryAttributeComponent.Get();
	TArray<FString> Lines;
	Lines.Add(TEXT("VITALS"));
	Lines.Add(FString::Printf(TEXT("Health: %.0f / %.0f"), Stats->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth), Stats->GetSecondaryAttribute(ESecondaryAttribute::MaxHealth)));
	Lines.Add(FString::Printf(TEXT("Mana: %.0f / %.0f"), Stats->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxMana), Stats->GetSecondaryAttribute(ESecondaryAttribute::MaxMana)));
	Lines.Add(FString::Printf(TEXT("Stamina: %.0f / %.0f"), Stats->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxStamina), Stats->GetSecondaryAttribute(ESecondaryAttribute::MaxStamina)));
	Lines.Add(TEXT(""));
	Lines.Add(TEXT("COMBAT"));
	Lines.Add(FString::Printf(TEXT("Armor: %.0f"), Stats->GetSecondaryAttribute(ESecondaryAttribute::Armor)));
	Lines.Add(FString::Printf(TEXT("Crit Chance: %.1f"), Stats->GetSecondaryAttribute(ESecondaryAttribute::CritChance)));
	const USkillBase* MainHandSkill = SkillManagerComponent ? SkillManagerComponent->GetMainHandSkill() : nullptr;
	const USkillDataAsset* AttackData = MainHandSkill ? MainHandSkill->SkillData.Get() : nullptr;
	const APawn* OwningPawn = GetOwningPlayerPawn();
	const UEquipmentComponent* Equipment = OwningPawn ? OwningPawn->FindComponentByClass<UEquipmentComponent>() : nullptr;
	const UEquipmentItem* MainHandItem = Equipment ? Equipment->GetEquippedItem(EEquipmentSlot::MainHand) : nullptr;
	const UItemDataAsset* MainHandData = MainHandItem ? MainHandItem->ItemData : nullptr;
	const UWeaponItem* MainHandWeapon = Cast<UWeaponItem>(MainHandItem);
	if (AttackData)
	{
		float Damage = FMath::IsFinite(AttackData->BaseDamage) ? FMath::Max(0.0f, AttackData->BaseDamage) : 0.0f;
		if (const USkillMeleeAttack* Melee = Cast<USkillMeleeAttack>(MainHandSkill))
		{
			Damage = Melee->GetAttackDamage();
		}
		else if (const USkillRangedAttack* Ranged = Cast<USkillRangedAttack>(MainHandSkill))
		{
			Damage = Ranged->GetAttackDamage();
		}
		else if (const USkillMagic* Magic = Cast<USkillMagic>(MainHandSkill))
		{
			Damage = Magic->GetAttackDamage();
		}
		const float Cooldown = MainHandSkill->GetEffectiveCooldownDuration();
		Lines.Add(FString::Printf(TEXT("Attack Damage: %.1f"), Damage));
		Lines.Add(Cooldown > 0.0f
			? FString::Printf(TEXT("Attack Speed: %.2f attacks/s"), 1.0f / Cooldown)
			: TEXT("Attack Speed: No cooldown"));
	}
	else
	{
		Lines.Add(TEXT("Attack Damage: None"));
		Lines.Add(TEXT("Attack Speed: None"));
	}
	Lines.Add(FString::Printf(TEXT("Move Speed: %.0f"), Stats->GetSecondaryAttribute(ESecondaryAttribute::MoveSpeed)));
	TArray<FString> Resistances;
	const UEnum* DamageEnum = StaticEnum<EDamageType>();
	for (const EDamageType Kind : { EDamageType::Physical, EDamageType::Fire, EDamageType::Cold,
		EDamageType::Lightning, EDamageType::Poison, EDamageType::Magic, EDamageType::Dark, EDamageType::Holy })
	{
		const float Percent = Stats->GetDamageResistancePercent(Kind);
		if (Percent <= 0.0f) continue;
		const FString Name = DamageEnum
			? DamageEnum->GetDisplayNameTextByValue(static_cast<int64>(Kind)).ToString()
			: TEXT("Damage");
		Resistances.Add(FString::Printf(TEXT("%s %.0f%%"), *Name, Percent));
	}
	Lines.Add(FString::Printf(TEXT("Resistances: %s"),
		Resistances.IsEmpty() ? TEXT("None") : *FString::Join(Resistances, TEXT(", "))));
	Lines.Add(TEXT(""));
	Lines.Add(FString::Printf(TEXT("Main Hand: %s"), MainHandData ? *MainHandData->ItemName.ToString() : TEXT("Empty")));
	const float WeaponBonus = MainHandWeapon ? MainHandWeapon->GetWeaponDamage() : MainHandData ? MainHandData->WeaponDamage : 0.0f;
	if (MainHandData && WeaponBonus > 0.0f)
	{
		const UEnum* DamageTypes = StaticEnum<EDamageType>();
		const FString DamageType = DamageTypes ? DamageTypes->GetDisplayNameTextByValue(static_cast<int64>(MainHandData->WeaponDamageType)).ToString() : TEXT("Physical");
		Lines.Add(FString::Printf(TEXT("Weapon Bonus: +%.0f %s"), WeaponBonus, *DamageType));
		if (MainHandData->bSoulboundGrowth && MainHandWeapon)
		{
			Lines.Add(FString::Printf(TEXT("Soulbound Rank: %d / %d (%d / %d defeats)"),
				MainHandWeapon->GetGrowthRank(), FMath::Max(1, MainHandData->MaxGrowthRank),
				MainHandWeapon->GetDefeatsTowardGrowth(), FMath::Max(1, MainHandData->DefeatsPerGrowthRank)));
		}
	}
	return FText::FromString(FString::Join(Lines, TEXT("\n")));
}
