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

	//////// CONSTRUCTOR ////////
	FGridNode() : WorldPosition(FVector::ZeroVector), IsCrossable(true)
	{
		
	}
};
