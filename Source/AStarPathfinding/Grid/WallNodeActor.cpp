#include "WallNodeActor.h"

AWallNodeActor::AWallNodeActor()
{
	NodeType = EGridActorType::Wall;
}

void AWallNodeActor::BeginPlay()
{
	Super::BeginPlay();
	SetupNodeColor(NodeType, ENodeState::Default);
}
