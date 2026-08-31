#include "RBHealthWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Styling/CoreStyle.h"

TSharedRef<SWidget> URBHealthWidget::RebuildWidget()
{
    UCanvasPanel* RootCanvas =
        WidgetTree->ConstructWidget<UCanvasPanel>(
            UCanvasPanel::StaticClass(),
            TEXT("RootCanvas")
        );

    WidgetTree->RootWidget = RootCanvas;

    HealthBar =
        WidgetTree->ConstructWidget<UProgressBar>(
            UProgressBar::StaticClass(),
            TEXT("HealthBar")
        );

    HealthBar->SetPercent(1.0f);

    HealthBar->SetFillColorAndOpacity(
        FLinearColor(0.0f, 1.0f, 0.0f, 1.0f)
    );

    UCanvasPanelSlot* HealthBarSlot =
        RootCanvas->AddChildToCanvas(HealthBar);

    if (HealthBarSlot)
    {
        HealthBarSlot->SetAnchors(
            FAnchors(0.0f, 0.0f)
        );

        HealthBarSlot->SetPosition(
            FVector2D(40.0f, 40.0f)
        );

        HealthBarSlot->SetSize(
            FVector2D(300.0f, 30.0f)
        );
    }

    HealthText =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("HealthText")
        );

    HealthText->SetText(
        FText::FromString(TEXT("HEALTH 100 / 100"))
    );

    HealthText->SetJustification(
        ETextJustify::Center
    );

    HealthText->SetColorAndOpacity(
        FSlateColor(FLinearColor::White)
    );

    HealthText->SetFont(
        FSlateFontInfo(
            FCoreStyle::GetDefaultFont(),
            18
        )
    );

    UCanvasPanelSlot* HealthTextSlot =
        RootCanvas->AddChildToCanvas(HealthText);

    if (HealthTextSlot)
    {
        HealthTextSlot->SetAnchors(
            FAnchors(0.0f, 0.0f)
        );

        HealthTextSlot->SetPosition(
            FVector2D(40.0f, 70.0f)
        );

        HealthTextSlot->SetSize(
            FVector2D(300.0f, 30.0f)
        );
    }

    return Super::RebuildWidget();
}

void URBHealthWidget::SetHealth(
    float CurrentHealth,
    float MaxHealth
)
{
    if (MaxHealth <= 0.0f)
    {
        return;
    }

    const float HealthPercent = FMath::Clamp(
        CurrentHealth / MaxHealth,
        0.0f,
        1.0f
    );

    if (HealthBar)
    {
        HealthBar->SetPercent(HealthPercent);
    }

    if (HealthText)
    {
        HealthText->SetText(
            FText::FromString(
                FString::Printf(
                    TEXT("HEALTH %.0f / %.0f"),
                    CurrentHealth,
                    MaxHealth
                )
            )
        );
    }
}