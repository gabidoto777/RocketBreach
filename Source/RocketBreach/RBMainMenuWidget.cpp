#include "RBMainMenuWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

TSharedRef<SWidget> URBMainMenuWidget::RebuildWidget()
{
    UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>();
    WidgetTree->RootWidget = RootCanvas;

    UVerticalBox* MenuBox = WidgetTree->ConstructWidget<UVerticalBox>();

    UCanvasPanelSlot* MenuSlot = RootCanvas->AddChildToCanvas(MenuBox);
    MenuSlot->SetAnchors(FAnchors(0.5f, 0.5f));
    MenuSlot->SetAlignment(FVector2D(0.5f, 0.5f));
    MenuSlot->SetPosition(FVector2D::ZeroVector);
    MenuSlot->SetAutoSize(true);

    UTextBlock* TitleText = WidgetTree->ConstructWidget<UTextBlock>();
    TitleText->SetText(FText::FromString(TEXT("ROCKET BREACH")));

    FSlateFontInfo TitleFont = TitleText->GetFont();
    TitleFont.Size = 48;
    TitleText->SetFont(TitleFont);

    if (UVerticalBoxSlot* TitleSlot = MenuBox->AddChildToVerticalBox(TitleText))
    {
        TitleSlot->SetHorizontalAlignment(HAlign_Center);
        TitleSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 30.0f));
    }

    PlayButton = WidgetTree->ConstructWidget<UButton>();

    UTextBlock* PlayText = WidgetTree->ConstructWidget<UTextBlock>();
    PlayText->SetText(FText::FromString(TEXT("PLAY")));
    PlayText->SetJustification(ETextJustify::Center);

    PlayButton->AddChild(PlayText);

    if (UVerticalBoxSlot* PlaySlot = MenuBox->AddChildToVerticalBox(PlayButton))
    {
        PlaySlot->SetPadding(FMargin(0.0f, 5.0f));
        PlaySlot->SetHorizontalAlignment(HAlign_Fill);
    }

    ExitButton = WidgetTree->ConstructWidget<UButton>();

    UTextBlock* ExitText = WidgetTree->ConstructWidget<UTextBlock>();
    ExitText->SetText(FText::FromString(TEXT("EXIT")));
    ExitText->SetJustification(ETextJustify::Center);

    ExitButton->AddChild(ExitText);

    if (UVerticalBoxSlot* ExitSlot = MenuBox->AddChildToVerticalBox(ExitButton))
    {
        ExitSlot->SetPadding(FMargin(0.0f, 5.0f));
        ExitSlot->SetHorizontalAlignment(HAlign_Fill);
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
        FName(TEXT("Lvl_RocketBreach_Prototype"))
    );
}

void URBMainMenuWidget::HandleExitClicked()
{
    if (APlayerController* PlayerController = GetOwningPlayer())
    {
        UKismetSystemLibrary::QuitGame(
            this,
            PlayerController,
            EQuitPreference::Quit,
            true
        );
    }
}