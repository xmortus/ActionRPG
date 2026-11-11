#include "UI/Common/DraggablePanelHandleWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"

void UDraggablePanelHandleWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UBorder* HitArea = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("DragHitArea"));
		HitArea->SetBrushColor(FLinearColor::Transparent);
		WidgetTree->RootWidget = HitArea;
	}
}

void UDraggablePanelHandleWidget::Configure(UUserWidget* InPanel, UWidget* InMoveTarget)
{
	PanelWidget = InPanel;
	MoveTarget = InMoveTarget;
}

void UDraggablePanelHandleWidget::AttachToCanvasTitle(UUserWidget* Panel, FName TitleName)
{
	if (!Panel || !Panel->WidgetTree || !Panel->GetOwningPlayer())
	{
		return;
	}
	UWidget* Title = Panel->WidgetTree->FindWidget(TitleName);
	UCanvasPanel* Canvas = Cast<UCanvasPanel>(Panel->WidgetTree->RootWidget);
	UCanvasPanelSlot* TitleSlot = Title ? Cast<UCanvasPanelSlot>(Title->Slot) : nullptr;
	const FName HandleName(TitleName.ToString() + TEXT("DragHandle"));
	if (!Canvas || !TitleSlot || Panel->WidgetTree->FindWidget(HandleName))
	{
		return;
	}
	UDraggablePanelHandleWidget* Handle = CreateWidget<UDraggablePanelHandleWidget>(Panel->GetOwningPlayer(), StaticClass(), HandleName);
	if (!Handle)
	{
		return;
	}
	Handle->Configure(Panel);
	UCanvasPanelSlot* HandleSlot = Canvas->AddChildToCanvas(Handle);
	HandleSlot->SetAnchors(TitleSlot->GetAnchors());
	HandleSlot->SetAlignment(TitleSlot->GetAlignment());
	HandleSlot->SetPosition(TitleSlot->GetPosition());
	HandleSlot->SetSize(TitleSlot->GetSize());
	HandleSlot->SetAutoSize(TitleSlot->GetAutoSize());
	HandleSlot->SetZOrder(TitleSlot->GetZOrder() + 10);
}

void UDraggablePanelHandleWidget::AttachToOverlayTitle(UUserWidget* Panel, FName TitleAreaName, FName MoveTargetName)
{
	if (!Panel || !Panel->WidgetTree || !Panel->GetOwningPlayer())
	{
		return;
	}
	UOverlay* TitleArea = Cast<UOverlay>(Panel->WidgetTree->FindWidget(TitleAreaName));
	UWidget* Target = Panel->WidgetTree->FindWidget(MoveTargetName);
	if (!TitleArea || !Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("Panel drag handle missing title or target: %s"), *GetNameSafe(Panel));
		return;
	}
	for (int32 Index = 0; Index < TitleArea->GetChildrenCount(); ++Index)
	{
		if (Cast<UDraggablePanelHandleWidget>(TitleArea->GetChildAt(Index)))
		{
			return;
		}
	}
	UDraggablePanelHandleWidget* Handle = CreateWidget<UDraggablePanelHandleWidget>(Panel->GetOwningPlayer(), StaticClass());
	if (Handle)
	{
		Handle->Configure(Panel, Target);
		UOverlaySlot* HandleSlot = TitleArea->AddChildToOverlay(Handle);
		HandleSlot->SetHorizontalAlignment(HAlign_Fill);
		HandleSlot->SetVerticalAlignment(VAlign_Fill);
		UE_LOG(LogTemp, Verbose, TEXT("Panel drag handle attached: %s"), *GetNameSafe(Panel));
	}
}

FReply UDraggablePanelHandleWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (PanelWidget && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		UE_LOG(LogTemp, Verbose, TEXT("Panel drag started: %s"), *GetNameSafe(PanelWidget));
		LastMousePosition = InMouseEvent.GetScreenSpacePosition();
		bDragging = true;
		return FReply::Handled().CaptureMouse(TakeWidget());
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UDraggablePanelHandleWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!bDragging || !PanelWidget)
	{
		return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
	}
	UWidget* Target = MoveTarget ? MoveTarget.Get() : PanelWidget.Get();
	const FGeometry TargetGeometry = Target->GetCachedGeometry();
	const FGeometry ViewportGeometry = UWidgetLayoutLibrary::GetViewportWidgetGeometry(this);
	const FVector2D ViewportMin = ViewportGeometry.GetAbsolutePosition();
	const FVector2D ViewportMax = ViewportMin + ViewportGeometry.GetAbsoluteSize();
	const FVector2D TargetMin = TargetGeometry.GetAbsolutePosition();
	const FVector2D TargetMax = TargetMin + TargetGeometry.GetAbsoluteSize();
	FVector2D Delta = InMouseEvent.GetScreenSpacePosition() - LastMousePosition;
	const FVector2D TargetSize = TargetGeometry.GetAbsoluteSize();
	const FVector2D ViewportSize = ViewportGeometry.GetAbsoluteSize();
	if (TargetSize.X <= ViewportSize.X)
	{
		Delta.X = FMath::Clamp(Delta.X, ViewportMin.X - TargetMin.X, ViewportMax.X - TargetMax.X);
	}
	else
	{
		// A panel wider than the game viewport keeps its title edge reachable.
		Delta.X = FMath::Clamp(TargetMin.X + Delta.X, ViewportMin.X, ViewportMax.X - 64.0f) - TargetMin.X;
	}
	if (TargetSize.Y <= ViewportSize.Y)
	{
		Delta.Y = FMath::Clamp(Delta.Y, ViewportMin.Y - TargetMin.Y, ViewportMax.Y - TargetMax.Y);
	}
	else
	{
		Delta.Y = FMath::Clamp(TargetMin.Y + Delta.Y, ViewportMin.Y, ViewportMax.Y - 48.0f) - TargetMin.Y;
	}
	const float Scale = TargetGeometry.Scale;
	if (Scale > KINDA_SMALL_NUMBER)
	{
		Target->SetRenderTranslation(Target->GetRenderTransform().Translation + Delta / Scale);
		UE_LOG(LogTemp, Verbose, TEXT("Panel drag moved: %s by %s"), *GetNameSafe(PanelWidget), *Delta.ToString());
	}
	LastMousePosition = InMouseEvent.GetScreenSpacePosition();
	return FReply::Handled();
}

FReply UDraggablePanelHandleWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bDragging && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bDragging = false;
		return FReply::Handled().ReleaseMouseCapture();
	}
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UDraggablePanelHandleWidget::NativeOnMouseCaptureLost(const FCaptureLostEvent& InCaptureLostEvent)
{
	bDragging = false;
	Super::NativeOnMouseCaptureLost(InCaptureLostEvent);
}
