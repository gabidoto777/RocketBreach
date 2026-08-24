#include "RBCrosshairWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"

TSharedRef<SWidget> URBCrosshairWidget::RebuildWidget()
{
    UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>();
    WidgetTree->RootWidget = RootCanvas;

    UTextBlock* CrosshairText = WidgetTree->ConstructWidget<UTextBlock>();

    CrosshairText->SetText(FText::FromString(TEXT("+")));
    CrosshairText->SetJustification(ETextJustify::Center);
    CrosshairText->SetColorAndOpacity(FSlateColor(FLinearColor::White));

    FSlateFontInfo CrosshairFont = CrosshairText->GetFont();
    CrosshairFont.Size = 28;
    CrosshairText->SetFont(CrosshairFont);

    UCanvasPanelSlot* CrosshairSlot =
        RootCanvas->AddChildToCanvas(CrosshairText);

    CrosshairSlot->SetAnchors(FAnchors(0.5f, 0.5f));
    CrosshairSlot->SetAlignment(FVector2D(0.5f, 0.5f));
    CrosshairSlot->SetPosition(FVector2D::ZeroVector);
    CrosshairSlot->SetAutoSize(true);

    return Super::RebuildWidget();
}