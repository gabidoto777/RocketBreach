#include "RBGameOverWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Styling/CoreStyle.h"

TSharedRef<SWidget> URBGameOverWidget::RebuildWidget()
{
    UCanvasPanel* RootCanvas =
        WidgetTree->ConstructWidget<UCanvasPanel>(
            UCanvasPanel::StaticClass(),
            TEXT("RootCanvas")
        );

    WidgetTree->RootWidget = RootCanvas;

    UVerticalBox* MenuBox =
        WidgetTree->ConstructWidget<UVerticalBox>(
            UVerticalBox::StaticClass(),
            TEXT("MenuBox")
        );

    UCanvasPanelSlot* MenuSlot =
        RootCanvas->AddChildToCanvas(MenuBox);

    if (MenuSlot)
    {
        MenuSlot->SetAnchors(
            FAnchors(0.5f, 0.5f)
        );

        MenuSlot->SetAlignment(
            FVector2D(0.5f, 0.5f)
        );

        MenuSlot->SetPosition(
            FVector2D::ZeroVector
        );

        MenuSlot->SetSize(
            FVector2D(420.0f, 520.0f)
        );
    }

    // -------------------------
    // GAME OVER
    // -------------------------

    UTextBlock* GameOverText =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("GameOverText")
        );

    GameOverText->SetText(
        FText::FromString(TEXT("GAME OVER"))
    );

    GameOverText->SetJustification(
        ETextJustify::Center
    );

    GameOverText->SetFont(
        FSlateFontInfo(
            FCoreStyle::GetDefaultFont(),
            36
        )
    );

    UVerticalBoxSlot* TitleSlot =
        MenuBox->AddChildToVerticalBox(GameOverText);

    if (TitleSlot)
    {
        TitleSlot->SetPadding(
            FMargin(20.0f, 10.0f, 20.0f, 25.0f)
        );

        TitleSlot->SetHorizontalAlignment(
            HAlign_Center
        );
    }

    // -------------------------
    // CURRENT RUN
    // -------------------------

    UTextBlock* CurrentRunTitle =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("CurrentRunTitle")
        );

    CurrentRunTitle->SetText(
        FText::FromString(TEXT("CURRENT RUN"))
    );

    CurrentRunTitle->SetJustification(
        ETextJustify::Center
    );

    CurrentRunTitle->SetFont(
        FSlateFontInfo(
            FCoreStyle::GetDefaultFont(),
            22
        )
    );

    UVerticalBoxSlot* CurrentRunTitleSlot =
        MenuBox->AddChildToVerticalBox(CurrentRunTitle);

    if (CurrentRunTitleSlot)
    {
        CurrentRunTitleSlot->SetPadding(
            FMargin(10.0f, 5.0f)
        );
    }

    EnemiesKilledText =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("EnemiesKilledText")
        );

    EnemiesKilledText->SetText(
        FText::FromString(
            TEXT("Enemies Killed: 0")
        )
    );

    EnemiesKilledText->SetJustification(
        ETextJustify::Center
    );

    MenuBox->AddChildToVerticalBox(
        EnemiesKilledText
    );

    WavesCompletedText =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("WavesCompletedText")
        );

    WavesCompletedText->SetText(
        FText::FromString(
            TEXT("Waves Completed: 0")
        )
    );

    WavesCompletedText->SetJustification(
        ETextJustify::Center
    );

    UVerticalBoxSlot* WavesSlot =
        MenuBox->AddChildToVerticalBox(
            WavesCompletedText
        );

    if (WavesSlot)
    {
        WavesSlot->SetPadding(
            FMargin(0.0f, 0.0f, 0.0f, 25.0f)
        );
    }

    // -------------------------
    // HIGH SCORE
    // -------------------------

    UTextBlock* HighScoreTitle =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("HighScoreTitle")
        );

    HighScoreTitle->SetText(
        FText::FromString(TEXT("HIGH SCORE"))
    );

    HighScoreTitle->SetJustification(
        ETextJustify::Center
    );

    HighScoreTitle->SetFont(
        FSlateFontInfo(
            FCoreStyle::GetDefaultFont(),
            22
        )
    );

    UVerticalBoxSlot* HighScoreTitleSlot =
        MenuBox->AddChildToVerticalBox(
            HighScoreTitle
        );

    if (HighScoreTitleSlot)
    {
        HighScoreTitleSlot->SetPadding(
            FMargin(10.0f, 5.0f)
        );
    }

    BestEnemiesKilledText =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("BestEnemiesKilledText")
        );

    BestEnemiesKilledText->SetText(
        FText::FromString(
            TEXT("Enemies Killed: 0")
        )
    );

    BestEnemiesKilledText->SetJustification(
        ETextJustify::Center
    );

    MenuBox->AddChildToVerticalBox(
        BestEnemiesKilledText
    );

    BestWavesCompletedText =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("BestWavesCompletedText")
        );

    BestWavesCompletedText->SetText(
        FText::FromString(
            TEXT("Waves Completed: 0")
        )
    );

    BestWavesCompletedText->SetJustification(
        ETextJustify::Center
    );

    UVerticalBoxSlot* BestWavesSlot =
        MenuBox->AddChildToVerticalBox(
            BestWavesCompletedText
        );

    if (BestWavesSlot)
    {
        BestWavesSlot->SetPadding(
            FMargin(0.0f, 0.0f, 0.0f, 30.0f)
        );
    }

    // -------------------------
    // RETRY
    // -------------------------

    RetryButton =
        WidgetTree->ConstructWidget<UButton>(
            UButton::StaticClass(),
            TEXT("RetryButton")
        );

    UTextBlock* RetryText =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("RetryText")
        );

    RetryText->SetText(
        FText::FromString(TEXT("RETRY"))
    );

    RetryText->SetJustification(
        ETextJustify::Center
    );

    RetryButton->AddChild(RetryText);

    UVerticalBoxSlot* RetrySlot =
        MenuBox->AddChildToVerticalBox(
            RetryButton
        );

    if (RetrySlot)
    {
        RetrySlot->SetPadding(
            FMargin(40.0f, 8.0f)
        );
    }

    // -------------------------
    // MAIN MENU
    // -------------------------

    MainMenuButton =
        WidgetTree->ConstructWidget<UButton>(
            UButton::StaticClass(),
            TEXT("MainMenuButton")
        );

    UTextBlock* MainMenuText =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("MainMenuText")
        );

    MainMenuText->SetText(
        FText::FromString(TEXT("MAIN MENU"))
    );

    MainMenuText->SetJustification(
        ETextJustify::Center
    );

    MainMenuButton->AddChild(MainMenuText);

    UVerticalBoxSlot* MainMenuSlot =
        MenuBox->AddChildToVerticalBox(
            MainMenuButton
        );

    if (MainMenuSlot)
    {
        MainMenuSlot->SetPadding(
            FMargin(40.0f, 8.0f)
        );
    }

    return Super::RebuildWidget();
}

void URBGameOverWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (RetryButton)
    {
        RetryButton->OnClicked.AddDynamic(
            this,
            &URBGameOverWidget::RetryGame
        );
    }

    if (MainMenuButton)
    {
        MainMenuButton->OnClicked.AddDynamic(
            this,
            &URBGameOverWidget::ReturnToMainMenu
        );
    }
}

void URBGameOverWidget::SetStats(
    int32 EnemiesKilled,
    int32 WavesCompleted,
    int32 BestEnemiesKilled,
    int32 BestWavesCompleted
)
{
    if (EnemiesKilledText)
    {
        EnemiesKilledText->SetText(
            FText::FromString(
                FString::Printf(
                    TEXT("Enemies Killed: %d"),
                    EnemiesKilled
                )
            )
        );
    }

    if (WavesCompletedText)
    {
        WavesCompletedText->SetText(
            FText::FromString(
                FString::Printf(
                    TEXT("Waves Completed: %d"),
                    WavesCompleted
                )
            )
        );
    }

    if (BestEnemiesKilledText)
    {
        BestEnemiesKilledText->SetText(
            FText::FromString(
                FString::Printf(
                    TEXT("Enemies Killed: %d"),
                    BestEnemiesKilled
                )
            )
        );
    }

    if (BestWavesCompletedText)
    {
        BestWavesCompletedText->SetText(
            FText::FromString(
                FString::Printf(
                    TEXT("Waves Completed: %d"),
                    BestWavesCompleted
                )
            )
        );
    }
}

void URBGameOverWidget::RetryGame()
{
    UGameplayStatics::OpenLevel(
        this,
        FName(TEXT("Lvl_RocketBreach_Prototype"))
    );
}

void URBGameOverWidget::ReturnToMainMenu()
{
    UGameplayStatics::OpenLevel(
        this,
        FName(TEXT("Lvl_RocketBreach_MainMenu"))
    );
}