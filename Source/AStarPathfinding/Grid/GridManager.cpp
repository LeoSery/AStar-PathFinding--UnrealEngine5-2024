#include "GridManager.h"
#include "DrawDebugHelpers.h"

AGridManager::AGridManager()
    : GridSizeX(DEFAULT_GRID_SIZE)
    , GridSizeY(DEFAULT_GRID_SIZE)
    , CellSize(DEFAULT_CELL_SIZE)
{
    PrimaryActorTick.bCanEverTick = false;
}

void AGridManager::BeginPlay()
{
    Super::BeginPlay();
    GridOrigin = GetActorLocation();
    Initialize();
    DrawGrid();
}

void AGridManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

int32 AGridManager::GetIndexFromXY(int32 X, int32 Y) const
{
    return Y * GridSizeX + X;
}

bool AGridManager::IsValidPos(int32 X, int32 Y) const
{
    return static_cast<uint32>(X) < static_cast<uint32>(GridSizeX) && 
           static_cast<uint32>(Y) < static_cast<uint32>(GridSizeY);
}

FGridNode& AGridManager::GetNode(int32 X, int32 Y)
{
    return Grid[GetIndexFromXY(X, Y)];
}

void AGridManager::Initialize()
{
    const int32 GridSize = GridSizeX * GridSizeY;
    Grid.Empty(GridSize);
    Grid.SetNum(GridSize);

    for (int32 y = 0; y < GridSizeY; y++)
    {
        const float YPos = GridOrigin.Y + (y * CellSize);
        
        for (int32 x = 0; x < GridSizeX; x++)
        {
            FGridNode& Node = GetNode(x, y);
            Node.WorldPosition = FVector(
                GridOrigin.X + (x * CellSize),
                YPos,
                GridOrigin.Z
            );
            Node.IsCrossable = true;
            Node.CostFromStart = 0.0f;
            Node.CostToGoal = 0.0f;
            Node.PreviousNode = nullptr;
        }
    }
}

void AGridManager::ClearDebugLines()
{
    if (GetWorld())
    {
        FlushPersistentDebugLines(GetWorld());
    }
}

void AGridManager::DrawGrid()
{
    if (!GetWorld()) return;

    ClearDebugLines();
    
    const FColor LineColor = FColor::Red;
    const float LineThickness = 3.0f;
    static const float LifeTime = -1.0f;
    static const bool bPersistent = true;
    
    for (int32 y = 0; y <= GridSizeY; y++)
    {
        const FVector StartPoint = GridOrigin + FVector(0.0f, y * CellSize, 0.0f);
        const FVector EndPoint = StartPoint + FVector(GridSizeX * CellSize, 0.0f, 0.0f);
        DrawDebugLine(GetWorld(), StartPoint, EndPoint, LineColor, bPersistent, LifeTime, 0, LineThickness);
    }
    
    for (int32 x = 0; x <= GridSizeX; x++)
    {
        const FVector StartPoint = GridOrigin + FVector(x * CellSize, 0.0f, 0.0f);
        const FVector EndPoint = StartPoint + FVector(0.0f, GridSizeY * CellSize, 0.0f);
        DrawDebugLine(GetWorld(), StartPoint, EndPoint, LineColor, bPersistent, LifeTime, 0, LineThickness);
    }
}
