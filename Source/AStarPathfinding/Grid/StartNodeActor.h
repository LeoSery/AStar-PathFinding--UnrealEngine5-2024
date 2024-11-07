#pragma once

#include "CoreMinimal.h"
#include "GridNodeActorBase.h"
#include "StartNodeActor.generated.h"

UCLASS()
class ASTARPATHFINDING_API AStartNodeActor : public AGridNodeActorBase
{
	GENERATED_BODY()

public:
	//////// CONSTRUCTOR ////////
	AStartNodeActor();

protected:
	//////// UNREAL LIFECYCLE ////////
	virtual void BeginPlay() override;
};
