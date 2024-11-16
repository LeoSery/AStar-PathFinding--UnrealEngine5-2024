#include "PathNodeActor.h"

APathNodeActor::APathNodeActor()
{
	NodeType = EGridActorType::None;
}

void APathNodeActor::BeginPlay()
{
	Super::BeginPlay();
	SetupNodeColor(NodeType, ENodeState::Default);
}

void APathNodeActor::SetPathNodeType(bool bIsFinalPath)
{
	if (bIsFinalPath)
	{
		UpdatePathFindingNodeColor(ENodeState::Path);
	}
	else
	{
		UpdatePathFindingNodeColor(ENodeState::Explored);
	}
}