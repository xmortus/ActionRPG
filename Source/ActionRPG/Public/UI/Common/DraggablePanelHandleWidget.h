#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DraggablePanelHandleWidget.generated.h"

class UWidget;
class UCanvasPanel;

/** Transparent title-area handle used by the game's movable panels. */
UCLASS()
class ACTIONRPG_API UDraggablePanelHandleWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Configure(UUserWidget* InPanel, UWidget* InMoveTarget = nullptr);
	static void AttachToCanvasTitle(UUserWidget* Panel, FName TitleName);
	static void AttachToOverlayTitle(UUserWidget* Panel, FName TitleAreaName, FName MoveTargetName);

protected:
	virtual void NativeOnInitialized() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseCaptureLost(const FCaptureLostEvent& InCaptureLostEvent) override;

private:
	UPROPERTY()
	TObjectPtr<UUserWidget> PanelWidget;

	UPROPERTY()
	TObjectPtr<UWidget> MoveTarget;

	FVector2D LastMousePosition = FVector2D::ZeroVector;
	bool bDragging = false;
};
