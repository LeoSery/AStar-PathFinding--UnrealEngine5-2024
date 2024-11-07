#pragma once

#include "CoreMinimal.h"
#include "GridNodeActorBase.h"
#include "WallNodeActor.generated.h"

UCLASS()
class ASTARPATHFINDING_API AWallNodeActor : public AGridNodeActorBase
{
	GENERATED_BODY()

public:
	//////// CONSTRUCTOR ////////
	AWallNodeActor();

protected:
	//////// UNREAL LIFECYCLE ////////
	virtual void BeginPlay() override;
};
