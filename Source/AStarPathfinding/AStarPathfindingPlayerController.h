#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "AStarPathfindingPlayerController.generated.h"

UCLASS()
class AAStarPathfindingPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AAStarPathfindingPlayerController();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
    UInputMappingContext* DefaultMappingContext;

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
};