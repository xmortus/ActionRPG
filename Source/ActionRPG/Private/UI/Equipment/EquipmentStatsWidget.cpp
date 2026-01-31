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
