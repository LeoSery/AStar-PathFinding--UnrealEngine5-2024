#include "GridManager.h"

AGridManager::AGridManager(): GridSizeX(0), GridSizeY(0), GridCellSize(0)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();

	// Save grid origin at the start
	GridOrigin = GetActorLocation();
	Initialize();
}

int32 AGridManager::GetIndexFromXY(int32 X, int32 Y) const
{
	return Y * GridSizeX + X;
}

bool AGridManager::IsValidPos(int32 X, int32 Y) const
{
	return X >= 0 && X < GridSizeX && Y >= 0 && Y < GridSizeY;
}

FGridNode& AGridManager::GetNode(int32 X, int32 Y)
{
	return Grid[GetIndexFromXY(X, Y)];
}

void AGridManager::Initialize()
{
	Grid.SetNum(GridSizeX * GridSizeY);

	for (int32 x = 0; x < GridSizeX; x++)
	{
		for (int32 y = 0; y < GridSizeY; y++)
		{
			FVector WorldPos = GridOrigin + FVector(x * GridCellSize, y * GridCellSize, 0.0f);
			
			FGridNode& Node = GetNode(x, y);
			
			Node.WorldPosition = WorldPos;
			Node.IsCrossable = true;
			Node.CostFromStart = 0.0f;
			Node.CostToGoal = 0.0f;
			Node.PreviousNode = nullptr;
		}
	}
}

void AGridManager::DrawGrid()
{
	// Lines configuration
	const FColor LineColor = FColor::Red;
	const float LineThickness = 5.0f;
	const float LifeTime = 5.0f;
	
	for (int32 y = 0; y <= GridSizeY; y++)
	{
		FVector StartPoint = GridOrigin + FVector(0, y * GridCellSize, 0);
		FVector EndPoint = GridOrigin + FVector(GridSizeX * GridCellSize, y * GridCellSize, 0);
		
		DrawDebugLine(GetWorld(), StartPoint, EndPoint, LineColor, false, LifeTime, 0, LineThickness);
	}
	
	for (int32 x = 0; x <= GridSizeX; x++)
	{
		FVector StartPoint = GridOrigin + FVector(x * GridCellSize, 0, 0);
		FVector EndPoint = GridOrigin + FVector(x * GridCellSize, GridSizeY * GridCellSize, 0);
		
		DrawDebugLine(GetWorld(), StartPoint, EndPoint, LineColor, false, LifeTime, 0, LineThickness);
	}
}

void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawGrid();
}
