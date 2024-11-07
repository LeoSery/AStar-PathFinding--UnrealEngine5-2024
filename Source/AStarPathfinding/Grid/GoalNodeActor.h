#pragma once

#include "CoreMinimal.h"
#include "GridNodeActorBase.h"
#include "GoalNodeActor.generated.h"

UCLASS()
class ASTARPATHFINDING_API AGoalNodeActor : public AGridNodeActorBase
{
	GENERATED_BODY()

public:
	//////// CONSTRUCTOR ////////
	AGoalNodeActor();

protected:
	//////// UNREAL LIFECYCLE ////////
	virtual void BeginPlay() override;
};
