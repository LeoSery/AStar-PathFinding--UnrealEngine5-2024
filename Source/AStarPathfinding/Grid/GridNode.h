#pragma once

#include "CoreMinimal.h"
#include "GridNode.generated.h"

USTRUCT(BlueprintType)
struct FGridNode
{
	GENERATED_BODY()

	// Position in the world
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector WorldPosition;
    
	// Is the node crossable
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsCrossable;
    
	// A* Node cost
	float CostFromStart;
	float CostToGoal;
	FGridNode* PreviousNode;

	FGridNode() : WorldPosition(FVector::ZeroVector), IsCrossable(true) , CostFromStart(0.f), CostToGoal(0.f), PreviousNode(nullptr)
	{
		
	}

	float GetFCost() const
	{
		return CostFromStart + CostToGoal;
	}
};
