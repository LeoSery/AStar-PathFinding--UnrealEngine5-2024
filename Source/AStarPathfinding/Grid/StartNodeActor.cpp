#include "StartNodeActor.h"

AStartNodeActor::AStartNodeActor()
{
	NodeType = EGridActorType::Start;
}

void AStartNodeActor::BeginPlay()
{
	Super::BeginPlay();
	SetupNodeColor(NodeType);
}
