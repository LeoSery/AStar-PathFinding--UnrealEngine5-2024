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

void AGridNodeActorBase::BeginPlay()
{
	Super::BeginPlay();
}

void AGridNodeActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGridNodeActorBase::SetupNodeColor(EGridActorType Type)
{
	if (UMaterial* Material = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial"))))
	{
		UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(Material, this);
        
		FLinearColor Color;
		switch(Type)
		{
		case EGridActorType::Start:
			Color = FLinearColor::Green;
			break;
		case EGridActorType::Goal:
			Color = FLinearColor::Blue;
			break;
		case EGridActorType::Wall:
			Color = FLinearColor(0.2f, 0.2f, 0.2f); // Grey
			break;
		}
        
		DynMaterial->SetVectorParameterValue(TEXT("Color"), Color);
		MeshComponent->SetMaterial(0, DynMaterial);
	}
}
