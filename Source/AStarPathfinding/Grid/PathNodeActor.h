#pragma once

#include "CoreMinimal.h"
#include "GridNodeActorBase.h"
#include "PathNodeActor.generated.h"

UCLASS()
class ASTARPATHFINDING_API APathNodeActor : public AGridNodeActorBase
{
	GENERATED_BODY()

public:
	/////// CONSTRUCTOR ////////
	APathNodeActor();

	//////// METHODS ////////
	/// Node methods
	void SetPathNodeType(bool bIsFinalPath);
	
protected:
	//////// UNREAL LIFECYCLE ////////
	virtual void BeginPlay() override;
};
