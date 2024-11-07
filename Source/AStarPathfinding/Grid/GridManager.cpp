#include "GridManager.h"
#include "DrawDebugHelpers.h"

AGridManager::AGridManager()
    : GridSizeX(DEFAULT_GRID_SIZE)
    , GridSizeY(DEFAULT_GRID_SIZE)
    , CellSize(DEFAULT_CELL_SIZE)
    , LastHighlightedNodeX(-1)
    , LastHighlightedNodeY(-1)
    , bHasHighlightedNode(false)
{
    PrimaryActorTick.bCanEverTick = false;
}

FVector AGridManager::GetWorldPositionFromCell(int32 X, int32 Y) const
{
    return GridOrigin + FVector((X + 0.5f) * CellSize,(Y + 0.5f) * CellSize,0.0f);
}

void AGridManager::DebugDrawCell(int32 X, int32 Y, FColor Color, float Duration)
{
    if (!IsValidPos(X, Y) || !GetWorld())
    {
        return;
    }

    FVector CenterPosition = GetWorldPositionFromCell(X, Y);
    FVector Extent(CellSize * 0.45f, CellSize * 0.45f, 50.0f);
    
    DrawDebugBox(
        GetWorld(),
        CenterPosition,
        Extent,
        Color,
        false,
        Duration,
        0,
        2.0f
    );
}

bool AGridManager::GetCellFromWorldPosition(const FVector& WorldPosition, int32& OutX, int32& OutY) const
{
    FVector RelativePosition = WorldPosition - GridOrigin;
    OutX = FMath::FloorToInt(RelativePosition.X / CellSize);
    OutY = FMath::FloorToInt(RelativePosition.Y / CellSize);
    return IsValidPos(OutX, OutY);
}

bool AGridManager::PlaceNodeActorInGrid(const FVector& WorldPosition)
{
    int32 GridX, GridY;
    if (!GetCellFromWorldPosition(WorldPosition, GridX, GridY))
    {
        return false;
    }
    
    RemoveExistingNodeActorAtCell(GridX, GridY);
    
    TSubclassOf<AGridNodeActorBase> ClassToSpawn = nullptr;
    switch (CurrentPlacementType)
    {
    case EGridActorType::Start:
        ClassToSpawn = StartNodeClass;
        break;
    case EGridActorType::Goal:
        ClassToSpawn = GoalNodeClass;
        break;
    case EGridActorType::Wall:
        ClassToSpawn = WallNodeClass;
        break;
    }

    if (!ClassToSpawn)
    {
        return false;
    }
    
    AGridNodeActorBase* NewActor = SpawnNodeActor(ClassToSpawn, GridX, GridY);
    if (!NewActor)
    {
        return false;
    }
    
    switch (CurrentPlacementType)
    {
    case EGridActorType::Start:
        if (StartNode) StartNode->Destroy();
        StartNode = NewActor;
        break;
            
    case EGridActorType::Goal:
        if (GoalNode) GoalNode->Destroy();
        GoalNode = NewActor;
        break;
            
    case EGridActorType::Wall:
        WallNodes.Add(FIntPoint(GridX, GridY), NewActor);
        GetNode(GridX, GridY).IsCrossable = false;
        break;
    }

    return true;
}

bool AGridManager::RemoveNodeActorFromGrid(const FVector& WorldPosition)
{
    int32 GridX, GridY;
    if (!GetCellFromWorldPosition(WorldPosition, GridX, GridY))
    {
        return false;
    }

    RemoveExistingNodeActorAtCell(GridX, GridY);
    return true;
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
    return static_cast<uint32>(X) < static_cast<uint32>(GridSizeX) && static_cast<uint32>(Y) < static_cast<uint32>(GridSizeY);
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

bool AGridManager::IsNodeAlreadyHighlighted(int32 X, int32 Y) const
{
    return bHasHighlightedNode && LastHighlightedNodeX == X && LastHighlightedNodeY == Y;
}

void AGridManager::UpdateHighlightedCell(int32 X, int32 Y)
{
    LastHighlightedNodeX = X;
    LastHighlightedNodeY = Y;
    bHasHighlightedNode = true;
}

void AGridManager::RemoveExistingNodeActorAtCell(int32 X, int32 Y)
{
    if (StartNode && StartNode->GridX == X && StartNode->GridY == Y)
    {
        StartNode->Destroy();
        StartNode = nullptr;
        return;
    }
    
    if (GoalNode && GoalNode->GridX == X && GoalNode->GridY == Y)
    {
        GoalNode->Destroy();
        GoalNode = nullptr;
        return;
    }
    
    FIntPoint Point(X, Y);
    if (AGridNodeActorBase* ExistingWall = WallNodes.FindRef(Point))
    {
        ExistingWall->Destroy();
        WallNodes.Remove(Point);
        GetNode(X, Y).IsCrossable = true;
    }
}

AGridNodeActorBase* AGridManager::SpawnNodeActor(TSubclassOf<AGridNodeActorBase> ActorClass, int32 X, int32 Y)
{
    if (!ActorClass || !GetWorld())
    {
        return nullptr;
    }

    FVector Location = GetWorldPositionFromCell(X, Y);
    FTransform SpawnTransform(FRotator::ZeroRotator, Location);

    AGridNodeActorBase* NewActor = GetWorld()->SpawnActor<AGridNodeActorBase>(ActorClass, SpawnTransform);
    if (NewActor)
    {
        NewActor->GridX = X;
        NewActor->GridY = Y;
    }

    return NewActor;
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
