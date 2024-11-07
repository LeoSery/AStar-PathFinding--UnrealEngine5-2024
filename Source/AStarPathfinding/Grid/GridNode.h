#pragma once

#include "CoreMinimal.h"
#include "GridNode.generated.h"

USTRUCT(BlueprintType)
struct FGridNode
{
	GENERATED_BODY()
	
	//////// FIELDS ////////
	//// Position fields
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector WorldPosition;

	//// State fields
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsCrossable;

	//// A* Pathfinding fields
	float CostFromStart;
	float CostToGoal;
	FGridNode* PreviousNode;

	//////// CONSTRUCTOR ////////
	FGridNode() : WorldPosition(FVector::ZeroVector), IsCrossable(true) , CostFromStart(0.f), CostToGoal(0.f), PreviousNode(nullptr)
	{
		
	}

	//////// METHODS ////////
	float GetFCost() const
	{
		return CostFromStart + CostToGoal;
	}
};
