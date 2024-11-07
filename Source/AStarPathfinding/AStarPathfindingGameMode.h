// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AStarPathfindingGameMode.generated.h"

UCLASS(minimalapi)
class AAStarPathfindingGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	//////// CONSTRUCTOR ////////
	AAStarPathfindingGameMode();

	//////// OVERRIDES ////////
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
};



