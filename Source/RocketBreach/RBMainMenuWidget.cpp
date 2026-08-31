#include "RBMainMenuWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/ScaleBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/Texture2D.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

TSharedRef<SWidget> URBMainMenuWidget::RebuildWidget()
{
    UCanvasPanel* RootCanvas =
        WidgetTree->ConstructWidget<UCanvasPanel>();

    WidgetTree->RootWidget = RootCanvas;

    // Background image
    UTexture2D* BackgroundTexture =
        LoadObject<UTexture2D>(
            nullptr,
            TEXT(
                "/Game/ROCKETBREACH/UI/T_Title.T_Title"
            )
        );

    if (BackgroundTexture)
    {
        UScaleBox* BackgroundScaleBox =
            WidgetTree->ConstructWidget<UScaleBox>();

        BackgroundScaleBox->SetStretch(
            EStretch::ScaleToFit
        );

        BackgroundScaleBox->SetStretchDirection(
            EStretchDirection::Both
        );

        UImage* BackgroundImage =
            WidgetTree->ConstructWidget<UImage>();

        BackgroundImage->SetBrushFromTexture(
            BackgroundTexture,
            true
        );

        BackgroundImage->SetVisibility(
            ESlateVisibility::HitTestInvisible
        );

        BackgroundScaleBox->AddChild(
            BackgroundImage
        );

        UCanvasPanelSlot* BackgroundSlot =
            RootCanvas->AddChildToCanvas(
                BackgroundScaleBox
            );

        BackgroundSlot->SetAnchors(
            FAnchors(
                0.0f,
                0.0f,
                1.0f,
                1.0f
            )
        );

        BackgroundSlot->SetOffsets(
            FMargin(
                0.0f,
                0.0f,
                0.0f,
                0.0f
            )
        );

        BackgroundSlot->SetZOrder(0);
    }

    // Menu buttons
    UVerticalBox* MenuBox =
        WidgetTree->ConstructWidget<UVerticalBox>();

    UCanvasPanelSlot* MenuSlot =
        RootCanvas->AddChildToCanvas(
            MenuBox
        );

    MenuSlot->SetAnchors(
        FAnchors(
            0.5f,
            0.78f
        )
    );

    MenuSlot->SetAlignment(
        FVector2D(
            0.5f,
            0.5f
        )
    );

    MenuSlot->SetPosition(
        FVector2D::ZeroVector
    );

    MenuSlot->SetSize(
        FVector2D(
            280.0f,
            150.0f
        )
    );

    MenuSlot->SetZOrder(1);

    // PLAY button
    PlayButton =
        WidgetTree->ConstructWidget<UButton>();

    UTextBlock* PlayText =
        WidgetTree->ConstructWidget<UTextBlock>();

    PlayText->SetText(
        FText::FromString(
            TEXT("PLAY")
        )
    );

    PlayText->SetJustification(
        ETextJustify::Center
    );

    FSlateFontInfo PlayFont =
        PlayText->GetFont();

    PlayFont.Size = 28;

    PlayText->SetFont(
        PlayFont
    );

    PlayButton->AddChild(
        PlayText
    );

    if (UVerticalBoxSlot* PlaySlot =
        MenuBox->AddChildToVerticalBox(
            PlayButton
        ))
    {
        PlaySlot->SetPadding(
            FMargin(
                0.0f,
                5.0f
            )
        );

        PlaySlot->SetHorizontalAlignment(
            HAlign_Fill
        );
    }

    // EXIT button
    ExitButton =
        WidgetTree->ConstructWidget<UButton>();

    UTextBlock* ExitText =
        WidgetTree->ConstructWidget<UTextBlock>();

    ExitText->SetText(
        FText::FromString(
            TEXT("EXIT")
        )
    );

    ExitText->SetJustification(
        ETextJustify::Center
    );

    FSlateFontInfo ExitFont =
        ExitText->GetFont();

    ExitFont.Size = 28;

    ExitText->SetFont(
        ExitFont
    );

    ExitButton->AddChild(
        ExitText
    );

    if (UVerticalBoxSlot* ExitSlot =
        MenuBox->AddChildToVerticalBox(
            ExitButton
        ))
    {
        ExitSlot->SetPadding(
            FMargin(
                0.0f,
                5.0f
            )
        );

        ExitSlot->SetHorizontalAlignment(
            HAlign_Fill
        );
    }

    return Super::RebuildWidget();
}

void URBMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (PlayButton)
    {
        PlayButton->OnClicked.AddDynamic(
            this,
            &URBMainMenuWidget::HandlePlayClicked
        );
    }

    if (ExitButton)
    {
        ExitButton->OnClicked.AddDynamic(
            this,
            &URBMainMenuWidget::HandleExitClicked
        );
    }
}

void URBMainMenuWidget::HandlePlayClicked()
{
    UGameplayStatics::OpenLevel(
        this,
        FName(
            TEXT("Lvl_RocketBreach_Prototype")
        )
    );
}

void URBMainMenuWidget::HandleExitClicked()
{
    if (APlayerController* PlayerController =
        GetOwningPlayer())
    {
        UKismetSystemLibrary::QuitGame(
            this,
            PlayerController,
            EQuitPreference::Quit,
            true
        );
    }
}