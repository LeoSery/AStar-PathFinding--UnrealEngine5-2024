#include "GoalNodeActor.h"

AGoalNodeActor::AGoalNodeActor()
{
	NodeType = EGridActorType::Goal;
}

void AGoalNodeActor::BeginPlay()
{
	Super::BeginPlay();
	SetupNodeColor(NodeType, ENodeState::Default);
}