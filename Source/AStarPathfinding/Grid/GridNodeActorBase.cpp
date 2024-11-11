#include "GridNodeActorBase.h"

AGridNodeActorBase::AGridNodeActorBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube"));
	if (CubeMeshAsset.Succeeded())
	{
		MeshComponent->SetStaticMesh(CubeMeshAsset.Object);
		MeshComponent->SetWorldScale3D(FVector(0.95f, 0.95f, 0.2f));
	}
}

void AGridNodeActorBase::SetupNodeColor(EGridActorType Type, ENodeState State)
{
	if (UMaterial* Material = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial"))))
	{
		UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(Material, this);
        
		FLinearColor Color;
        
		switch(State)
		{
		case ENodeState::ToExplore:
			Color = FLinearColor::Yellow;
			break;
		case ENodeState::Explored:
			Color = FLinearColor(0.5f, 0.5f, 0.5f); // Gris
			break;
		case ENodeState::Path:
			Color = FLinearColor(0.0f, 1.0f, 0.0f); // Vert vif
			break;
		case ENodeState::Neighbor:
			Color = FLinearColor(0.0f, 1.0f, 1.0f); // Cyan
			break;
		default:
				switch(Type)
				{
		case EGridActorType::Start:
			Color = FLinearColor::Green;
					break;
		case EGridActorType::Goal:
			Color = FLinearColor::Blue;
					break;
		case EGridActorType::Wall:
			Color = FLinearColor(0.2f, 0.2f, 0.2f); // Gris foncé
					break;
		default:
			Color = FLinearColor::White;
				}
		}
        
		NodeState = State;
		DynMaterial->SetVectorParameterValue(TEXT("Color"), Color);
		MeshComponent->SetMaterial(0, DynMaterial);
	}
}

void AGridNodeActorBase::BeginPlay()
{
	Super::BeginPlay();
}

void AGridNodeActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
