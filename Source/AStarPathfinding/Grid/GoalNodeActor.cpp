#include "GoalNodeActor.h"

AGoalNodeActor::AGoalNodeActor()
{
	NodeType = EGridActorType::Goal;
}

void AGoalNodeActor::BeginPlay()
{
	NodeType = EGridActorType::Goal;
	Super::BeginPlay();
	SetupNodeColor(NodeType, ENodeState::Default);
}