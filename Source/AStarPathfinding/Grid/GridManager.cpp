#include "GridManager.h"
#include "DrawDebugHelpers.h"
#include "AStarPathfinding/Solver/PathFinder.h"

AGridManager::AGridManager()
    : GridSizeX(DEFAULT_GRID_SIZE)
      , GridSizeY(DEFAULT_GRID_SIZE)
      , CellSize(DEFAULT_CELL_SIZE)
      , InteractionDistance(0), InteractionRate(0), CurrentPlacementType(), LastHighlightedNodeX(-1)
      , LastHighlightedNodeY(-1)
      , bHasHighlightedNode(false), StartNode(nullptr), GoalNode(nullptr)
{
    PrimaryActorTick.bCanEverTick = false;

    FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMesh"));
    RootComponent = FloorMesh;
    
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube"));
    if (CubeMeshAsset.Succeeded())
    {
        FloorMesh->SetStaticMesh(CubeMeshAsset.Object);
        
        FloorMesh->SetCollisionProfileName(TEXT("Default"));
        FloorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        FloorMesh->SetGenerateOverlapEvents(false);
        FloorMesh->CanCharacterStepUpOn = ECB_Yes;
    }
}

bool AGridManager::StaticIsValidPos(int32 X, int32 Y, int32 GridSizeX, int32 GridSizeY)
{
    return static_cast<uint32>(X) < static_cast<uint32>(GridSizeX) && static_cast<uint32>(Y) < static_cast<uint32>(GridSizeY);
}

int32 AGridManager::StaticGetIndexFromXY(int32 X, int32 Y, int32 GridSizeX)
{
    return Y * GridSizeX + X;
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

    UpdateHighlightedCell(X, Y);
}

FVector AGridManager::GetHighlightedCellWorldPosition() const
{
    return GetWorldPositionFromCell(LastHighlightedNodeX, LastHighlightedNodeY);
}

EGridActorType AGridManager::GetNodeTypeAtPosition(const FVector& WorldPosition) const
{
    int32 GridX, GridY;
    if (!GetCellFromWorldPosition(WorldPosition, GridX, GridY))
    {
        return EGridActorType::None;
    }
    
    if (StartNode && StartNode->GridX == GridX && StartNode->GridY == GridY)
    {
        return EGridActorType::Start;
    }
    
    if (GoalNode && GoalNode->GridX == GridX && GoalNode->GridY == GridY)
    {
        return EGridActorType::Goal;
    }
    
    if (WallNodes.Contains(FIntPoint(GridX, GridY)))
    {
        return EGridActorType::Wall;
    }

    return EGridActorType::None;
}

bool AGridManager::GetCellFromWorldPosition(const FVector& WorldPosition, int32& OutX, int32& OutY) const
{
    FVector RelativePosition = WorldPosition - GridOrigin;
    OutX = FMath::FloorToInt(RelativePosition.X / CellSize);
    OutY = FMath::FloorToInt(RelativePosition.Y / CellSize);
    return IsValidPos(OutX, OutY);
}

bool AGridManager::ToggleNodeActorInGrid(const FVector& WorldPosition)
{
    int32 GridX, GridY;
    if (!GetCellFromWorldPosition(WorldPosition, GridX, GridY))
    {
        return false;
    }

    FIntPoint Point(GridX, GridY);
    
    EGridActorType ExistingType = GetNodeTypeAtPosition(WorldPosition);
    if (ExistingType == CurrentPlacementType)
    {
        RemoveExistingNodeActorAtCell(GridX, GridY);
        OnGridChanged.Broadcast();
        UpdatePathfinding();
        return true;
    }
    
    RemoveExistingNodeActorAtCell(GridX, GridY);
    GetNode(GridX, GridY).IsCrossable = true;
    
    TSubclassOf<AGridNodeActorBase> ClassToSpawn = nullptr;
    switch (CurrentPlacementType)
    {
    case EGridActorType::Start:
        if (StartNode) StartNode->Destroy();
        ClassToSpawn = StartNodeClass;
        break;
    case EGridActorType::Goal:
        if (GoalNode) GoalNode->Destroy();
        ClassToSpawn = GoalNodeClass;
        break;
    case EGridActorType::Wall:
        ClassToSpawn = WallNodeClass;
        break;
    default:
        return false;
    }

    AGridNodeActorBase* NewActor = SpawnNodeActor(ClassToSpawn, GridX, GridY);
    if (!NewActor) return false;
    
    switch (CurrentPlacementType)
    {
    case EGridActorType::Start:
        StartNode = NewActor;
        break;
    case EGridActorType::Goal:
        GoalNode = NewActor;
        break;
    case EGridActorType::Wall:
        WallNodes.Add(Point, NewActor);
        GetNode(GridX, GridY).IsCrossable = false;
        break;
    }

    OnGridChanged.Broadcast();
    UpdatePathfinding();
    return true;
}

// void AGridManager::BeginPlay()
// {
//     Super::BeginPlay();
//     GridOrigin = GetActorLocation();
//     Initialize();
//     DrawGrid();
// }

void AGridManager::BeginPlay()
{
    Super::BeginPlay();
    
    if (FloorMesh)
    {
        FString CollisionProfile = FloorMesh->GetCollisionProfileName().ToString();
        ECollisionEnabled::Type CollisionType = FloorMesh->GetCollisionEnabled();
        
        GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, 
            FString::Printf(TEXT("Collision Profile: %s"), *CollisionProfile));
        GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, 
            FString::Printf(TEXT("Collision Enabled: %d"), static_cast<int32>(CollisionType)));
        
        FloorMesh->SetCollisionProfileName(TEXT("BlockAll"));
        FloorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("No FloorMesh in BeginPlay!"));
    }
    
    GridOrigin = FVector::ZeroVector;
    Initialize();
    DrawGrid();
}

void AGridManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

int32 AGridManager::GetIndexFromXY(int32 X, int32 Y) const
{
    return StaticGetIndexFromXY(X, Y, GridSizeX);
}

bool AGridManager::IsValidPos(int32 X, int32 Y) const
{
    return StaticIsValidPos(X, Y, GridSizeX, GridSizeY);
}

FGridNode& AGridManager::GetNode(int32 X, int32 Y)
{
    return Grid[GetIndexFromXY(X, Y)];
}

void AGridManager::Initialize()
{
    GridOrigin = FVector::ZeroVector;
    
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
        }
    }
    
    UpdateFloorMesh();
}

void AGridManager::ClearDebugLines()
{
    if (GetWorld())
    {
        FlushPersistentDebugLines(GetWorld());
    }
}

void AGridManager::UpdateFloorMesh()
{
    if (FloorMesh)
    {
        const float TotalWidth = GridSizeX * CellSize;
        const float TotalHeight = GridSizeY * CellSize;
        
        FloorMesh->SetWorldScale3D(FVector(TotalWidth/100.0f, TotalHeight/100.0f, 0.1f));
        
        FloorMesh->SetRelativeLocation(FVector(TotalWidth/2.0f, TotalHeight/2.0f, -10.0f));
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

AGridNodeActorBase* AGridManager::GetNodeActorAtCell(int32 X, int32 Y) const
{
    if (StartNode && StartNode->GridX == X && StartNode->GridY == Y)
    {
        return StartNode;
    }
        
    if (GoalNode && GoalNode->GridX == X && GoalNode->GridY == Y)
    {
        return GoalNode;
    }
        
    return WallNodes.FindRef(FIntPoint(X, Y));
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

void AGridManager::UpdatePathfinding()
{
    ClearAllNodeStates();
    CurrentPath.Empty();
    ExploredNodes.Empty();
    
    if (!StartNode || !GoalNode) return;
    
    CurrentPath = PathFinder::Compute(
        Grid,
        GridSizeX,
        GridSizeY,
        StartNode->GridX,
        StartNode->GridY,
        GoalNode->GridX,
        GoalNode->GridY,
        CellSize
    );
    
    UpdateNodeStates();
    OnPathUpdated.Broadcast(CurrentPath, ExploredNodes);
}

void AGridManager::UpdateNodeStates()
{
    for (const auto& PathPoint : CurrentPath)
    {
        int32 X, Y;
        if (GetCellFromWorldPosition(PathPoint, X, Y))
        {
            if (AGridNodeActorBase* Node = GetNodeActorAtCell(X, Y))
            {
                Node->SetupNodeColor(Node->NodeType, ENodeState::Path);
            }
        }
    }
}

void AGridManager::ClearAllNodeStates()
{
    if (StartNode)
    {
        StartNode->SetupNodeColor(EGridActorType::Start);
    }
    
    if (GoalNode)
    {
        GoalNode->SetupNodeColor(EGridActorType::Goal);
    }
    
    for (auto& Pair : WallNodes)
    {
        if (Pair.Value)
        {
            Pair.Value->SetupNodeColor(EGridActorType::Wall);
        }
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
