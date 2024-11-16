#include "GridNodeActorBase.h"

AGridNodeActorBase::AGridNodeActorBase()
{
	PrimaryActorTick.bCanEverTick = false;

    NodeType = EGridActorType::None;
	
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
    if (bIsBaseColorSet && State == ENodeState::Default && Type != NodeType)
    {
        return;
    }

    if (UMaterial* Material = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial"))))
    {
        UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(Material, this);
        
        FLinearColor Color;
        
        if (State == ENodeState::Default)
        {
            switch(Type)
            {
            case EGridActorType::Start:
                Color = FLinearColor(0.0f, 0.0f, 1.0f); // RoyalBlue
                break;
            case EGridActorType::Goal:
                Color = FLinearColor(1.0f, 0.686f, 0.0f); // Gold
                break;
            case EGridActorType::Wall:
                Color = FLinearColor(0.2f, 0.2f, 0.2f); // DarkGray
                break;
            default:
                Color = FLinearColor::White;
            }
            bIsBaseColorSet = true;
        }
        else
        {
            switch(State)
            {
            case ENodeState::ToExplore:
                Color = FLinearColor(0.941f, 0.502f, 0.502f); // Orange
                break;
            case ENodeState::Explored:
                Color = FLinearColor(1.0f, 1.0f, 0.0f); // Yellow
                break;
            case ENodeState::Path:
                Color = FLinearColor(0.565f, 0.933f, 0.565f); // LightGreen
                break;
            case ENodeState::Neighbor:
                Color = FLinearColor(0.0f, 1.0f, 1.0f); // LightBlue
                break;
            default:
                Color = FLinearColor::White;
            }
        }
        
        NodeState = State;
        if (State == ENodeState::Default)
        {
            NodeType = Type;
        }
        
        DynMaterial->SetVectorParameterValue(TEXT("Color"), Color);
        MeshComponent->SetMaterial(0, DynMaterial);
    }
}

void AGridNodeActorBase::UpdatePathFindingNodeColor(ENodeState State)
{
    if (NodeType == EGridActorType::Wall)
    {
        return;
    }

    if (UMaterialInstanceDynamic* DynMaterial = Cast<UMaterialInstanceDynamic>(MeshComponent->GetMaterial(0)))
    {
        FLinearColor Color;
        
        if (NodeType == EGridActorType::Start || NodeType == EGridActorType::Goal)
        {
            switch(NodeType)
            {
            case EGridActorType::Start:
                Color = FLinearColor(0.0f, 0.0f, 1.0f); // RoyalBlue
                break;
            case EGridActorType::Goal:
                Color = FLinearColor(1.0f, 0.686f, 0.0f); // Gold
                break;
            }
        }
        else
        {
            switch(State)
            {
            case ENodeState::ToExplore:
                Color = FLinearColor(0.941f, 0.502f, 0.502f); // Orange
                break;
            case ENodeState::Explored:
                Color = FLinearColor(1.0f, 0.0f, 0.0f); // Red
                break;
            case ENodeState::Path:
                Color = FLinearColor(0.0f, 1.0f, 0.0f); // LightGreen
                break;
            case ENodeState::Neighbor:
                Color = FLinearColor(0.0f, 1.0f, 1.0f); // LightBlue
                break;
            default:
                Color = FLinearColor::White;
            }
        }

        NodeState = State;
        DynMaterial->SetVectorParameterValue(TEXT("Color"), Color);
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
