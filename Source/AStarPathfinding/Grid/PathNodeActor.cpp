#include "PathNodeActor.h"

APathNodeActor::APathNodeActor()
{
	NodeType = EGridActorType::Path;
}

void APathNodeActor::BeginPlay()
{
	Super::BeginPlay();
	SetupNodeColor(NodeType);
}

void APathNodeActor::SetPathNodeType(bool bIsFinalPath)
{
	if (UMaterialInstanceDynamic* DynMaterial = Cast<UMaterialInstanceDynamic>(MeshComponent->GetMaterial(0)))
	{
		const FLinearColor Color = bIsFinalPath ? FLinearColor(0.0f, 1.0f, 0.0f) : FLinearColor::Yellow;
		DynMaterial->SetVectorParameterValue(TEXT("Color"), Color);
	}
}