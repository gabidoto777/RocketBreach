#include "RBPauseMenuWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

TSharedRef<SWidget> URBPauseMenuWidget::RebuildWidget()
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
    TitleText->SetText(FText::FromString(TEXT("PAUSED")));

    FSlateFontInfo TitleFont = TitleText->GetFont();
    TitleFont.Size = 48;
    TitleText->SetFont(TitleFont);

    if (UVerticalBoxSlot* TitleSlot = MenuBox->AddChildToVerticalBox(TitleText))
    {
        TitleSlot->SetHorizontalAlignment(HAlign_Center);
        TitleSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 30.0f));
    }

    ResumeButton = WidgetTree->ConstructWidget<UButton>();
    UTextBlock* ResumeText = WidgetTree->ConstructWidget<UTextBlock>();
    ResumeText->SetText(FText::FromString(TEXT("RESUME")));
    ResumeButton->AddChild(ResumeText);

    if (UVerticalBoxSlot* ResumeSlot = MenuBox->AddChildToVerticalBox(ResumeButton))
    {
        ResumeSlot->SetPadding(FMargin(0.0f, 8.0f));
        ResumeSlot->SetHorizontalAlignment(HAlign_Fill);
    }

    MainMenuButton = WidgetTree->ConstructWidget<UButton>();
    UTextBlock* MainMenuText = WidgetTree->ConstructWidget<UTextBlock>();
    MainMenuText->SetText(FText::FromString(TEXT("MAIN MENU")));
    MainMenuButton->AddChild(MainMenuText);

    if (UVerticalBoxSlot* MainMenuSlot = MenuBox->AddChildToVerticalBox(MainMenuButton))
    {
        MainMenuSlot->SetPadding(FMargin(0.0f, 8.0f));
        MainMenuSlot->SetHorizontalAlignment(HAlign_Fill);
    }

    ExitButton = WidgetTree->ConstructWidget<UButton>();
    UTextBlock* ExitText = WidgetTree->ConstructWidget<UTextBlock>();
    ExitText->SetText(FText::FromString(TEXT("EXIT")));
    ExitButton->AddChild(ExitText);

    if (UVerticalBoxSlot* ExitSlot = MenuBox->AddChildToVerticalBox(ExitButton))
    {
        ExitSlot->SetPadding(FMargin(0.0f, 8.0f));
        ExitSlot->SetHorizontalAlignment(HAlign_Fill);
    }

    return Super::RebuildWidget();
}

void URBPauseMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ResumeButton->OnClicked.AddDynamic(
        this,
        &URBPauseMenuWidget::HandleResumeClicked
    );

    MainMenuButton->OnClicked.AddDynamic(
        this,
        &URBPauseMenuWidget::HandleMainMenuClicked
    );

    ExitButton->OnClicked.AddDynamic(
        this,
        &URBPauseMenuWidget::HandleExitClicked
    );
}

void URBPauseMenuWidget::HandleResumeClicked()
{
    if (APlayerController* PlayerController = GetOwningPlayer())
    {
        PlayerController->SetPause(false);
        PlayerController->bShowMouseCursor = false;

        FInputModeGameOnly InputMode;
        PlayerController->SetInputMode(InputMode);

        RemoveFromParent();
    }
}

void URBPauseMenuWidget::HandleMainMenuClicked()
{
    UGameplayStatics::OpenLevel(
        this,
        FName(TEXT("Lvl_RocketBreach_MainMenu"))
    );
}

void URBPauseMenuWidget::HandleExitClicked()
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