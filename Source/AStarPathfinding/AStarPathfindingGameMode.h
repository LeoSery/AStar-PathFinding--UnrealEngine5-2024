// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Grid/GridManager.h"
#include "AStarPathfindingGameMode.generated.h"

UCLASS(minimalapi)
class AAStarPathfindingGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	//////// CONSTRUCTOR ////////
	AAStarPathfindingGameMode();

	//////// OVERRIDES ////////
	virtual void BeginPlay() override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

protected:
	//////// FIELDS ////////
	UPROPERTY()
	AGridManager* GridManager;

	//////// METHODS ////////
	//// Delegates methods
	UFUNCTION()
	void OnGridStateChanged();

	UFUNCTION()
	void OnPathReCalculated(const TArray<FVector>& Path, const TArray<FVector>& ExploredNodes);

private:
	//////// METHODS ////////
	//// Grid methods
	void InitializeGrid();

	UPROPERTY()
	TSubclassOf<AGridManager> GridManagerClass;
};
