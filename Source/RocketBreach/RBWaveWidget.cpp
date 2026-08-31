#include "RBWaveWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Styling/CoreStyle.h"

TSharedRef<SWidget> URBWaveWidget::RebuildWidget()
{
    UCanvasPanel* RootCanvas =
        WidgetTree->ConstructWidget<UCanvasPanel>(
            UCanvasPanel::StaticClass(),
            TEXT("RootCanvas")
        );

    WidgetTree->RootWidget = RootCanvas;

    WaveText =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("WaveText")
        );

    WaveText->SetJustification(
        ETextJustify::Center
    );

    WaveText->SetFont(
        FSlateFontInfo(
            FCoreStyle::GetDefaultFont(),
            36
        )
    );

    UCanvasPanelSlot* TextSlot =
        RootCanvas->AddChildToCanvas(WaveText);

    TextSlot->SetAnchors(
        FAnchors(0.5f, 0.35f)
    );

    TextSlot->SetAlignment(
        FVector2D(0.5f, 0.5f)
    );

    TextSlot->SetSize(
        FVector2D(600.0f, 80.0f)
    );

    WaveText->SetVisibility(
        ESlateVisibility::Collapsed
    );

    return Super::RebuildWidget();
}

void URBWaveWidget::ShowWaveComplete(
    int32 WaveNumber
)
{
    if (!WaveText)
    {
        return;
    }

    WaveText->SetText(
        FText::FromString(
            FString::Printf(
                TEXT("WAVE %d COMPLETE"),
                WaveNumber
            )
        )
    );

    WaveText->SetVisibility(
        ESlateVisibility::Visible
    );
}

void URBWaveWidget::HideMessage()
{
    if (WaveText)
    {
        WaveText->SetVisibility(
            ESlateVisibility::Collapsed
        );
    }
}