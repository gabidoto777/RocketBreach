#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RBPlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class ROCKETBREACH_API ARBPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ARBPlayerController();

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* MouseLookMappingContext;
};