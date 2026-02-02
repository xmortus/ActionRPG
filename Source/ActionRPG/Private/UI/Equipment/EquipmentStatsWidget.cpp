// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Equipment/EquipmentStatsWidget.h"
#include "Components/Progression/AttributeComponent.h"
#include "Components/Progression/SecondaryAttributeComponent.h"
#include "GameFramework/Pawn.h"

void UEquipmentStatsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindComponents();
	OnStatsUpdated();
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
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentStatsWidget - SecondaryAttributeComponent not found."));
	}
}

void UEquipmentStatsWidget::HandlePrimaryAttributeChanged(EPrimaryAttribute Attribute, float NewValue, float OldValue)
{
	OnStatsUpdated();
}

void UEquipmentStatsWidget::HandleSecondaryAttributeChanged(ESecondaryAttribute Attribute, float NewValue)
{
	OnStatsUpdated();
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

		if (Enum && Enum->HasMetaData(TEXT("Hidden"), Index))
		{
			continue;
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
		return FText::FromString(TEXT("None"));
	}

	const UEnum* Enum = StaticEnum<ESecondaryAttribute>();
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

		if (Enum && Enum->HasMetaData(TEXT("Hidden"), Index))
		{
			continue;
		}

		const ESecondaryAttribute Attribute = Enum ? static_cast<ESecondaryAttribute>(Enum->GetValueByIndex(Index)) : ESecondaryAttribute::MaxHealth;
		const float Value = SecondaryAttributeComponent->GetSecondaryAttribute(Attribute);
		const FString DisplayName = Enum ? Enum->GetDisplayNameTextByIndex(Index).ToString() : TEXT("Attribute");
		Lines.Add(FString::Printf(TEXT("%s: %.0f"), *DisplayName, Value));
	}

	if (Lines.Num() == 0)
	{
		return FText::FromString(TEXT("None"));
	}

	return FText::FromString(FString::Join(Lines, TEXT("\n")));
}
