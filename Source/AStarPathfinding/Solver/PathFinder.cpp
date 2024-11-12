#include "PathFinder.h"
#include "AStarPathfinding/Grid/GridManager.h"

// Represents the 8 neighbors of a node in grid space:
//   NW   N   NE
//   W   [C]   E
//   SW   S   SE
const TArray<TPair<int32, int32>> PathFinder::Directions =
{
    {-1, 1},  {0, 1},   {1, 1},    // North-West, North, North-East
    {-1, 0},             {1, 0},      // West, [Current], East
    {-1, -1}, {0, -1},  {1, -1},   // South-West, South, South-East
};

TArray<FVector> PathFinder::Compute(const TArray<FGridNode>& Grid, int32 GridSizeX, int32 GridSizeY, int32 StartX,
    int32 StartY, int32 GoalX, int32 GoalY, float CellSize, TArray<FVector>& OutExploredNodes)
{
    OutExploredNodes.Empty();
    
    if (!ValidateInputs(StartX, StartY, GoalX, GoalY, GridSizeX, GridSizeY))
    {
        return TArray<FVector>(); // Invalid Start or Goal > Impossible path
    }
    
    // Initialize nodes
    TArray<FPathNode> PathNodes;
    InitializePathNodes(PathNodes, GridSizeX, GridSizeY);
    
    TArray<FPathNode*> NodesToExplore;
    SetupStartNode(PathNodes, NodesToExplore, StartX, StartY, GoalX, GoalY, GridSizeX);
    
    // Safety > prevent infinite loop
    const int32 MaxIterations = GridSizeX * GridSizeY;
    int32 IterationCount = 0;

    // Main pathfinding loop
    while (!NodesToExplore.IsEmpty())
    {
        if (++IterationCount > MaxIterations)
        {
            UE_LOG(LogTemp, Warning, TEXT("PathFinder : Maximum iterations reached, path not found"));
            return TArray<FVector>();
        }
        
        FPathNode* CurrentNode = FindNodeWithLowestCost(NodesToExplore);

        if (!CurrentNode->IsExplored)
        {
            const FVector WorldPos(
                (CurrentNode->X + 0.5f) * CellSize,
                (CurrentNode->Y + 0.5f) * CellSize,
                0.0f
            );
            OutExploredNodes.Add(WorldPos);
        }
        
        if (IsGoalNode(CurrentNode, GoalX, GoalY))
        {
            return ReconstructPathToStart(CurrentNode, CellSize);
        }
        
        CurrentNode->IsExplored = true;
        
        // Check all possible directions
        for (const auto& Direction : Directions)
        {
            ProcessNeighbor(Direction, CurrentNode, PathNodes, Grid,
                GridSizeX, GridSizeY, GoalX, GoalY, NodesToExplore);
        }
    }
    
    return TArray<FVector>();
}

bool PathFinder::ValidateInputs(int32 StartX, int32 StartY, int32 GoalX, int32 GoalY, int32 GridSizeX, int32 GridSizeY)
{
    return AGridManager::StaticIsValidPos(StartX, StartY, GridSizeX, GridSizeY) 
        && AGridManager::StaticIsValidPos(GoalX, GoalY, GridSizeX, GridSizeY);
}

void PathFinder::InitializePathNodes(TArray<FPathNode>& PathNodes, int32 GridSizeX, int32 GridSizeY)
{
    PathNodes.SetNum(GridSizeX * GridSizeY); // Allow memeory for all nodes
    
    // Initialize each node with default values
    for (int32 Y = 0; Y < GridSizeY; ++Y)
    {
        for (int32 X = 0; X < GridSizeX; ++X)
        {
            const int32 Index = AGridManager::StaticGetIndexFromXY(X, Y, GridSizeX);
            InitializeNode(PathNodes[Index], X, Y);
        }
    }
}

void PathFinder::InitializeNode(FPathNode& Node, int32 X, int32 Y)
{
    Node.X = X;
    Node.Y = Y;
    Node.CostFromStart = MAX_int32; // default value for non-explored nodes
    Node.EstimatedCostToGoal = 0;
    Node.PreviousNode = nullptr;
    Node.IsExplored = false;
}

void PathFinder::SetupStartNode(TArray<FPathNode>& PathNodes, TArray<FPathNode*>& NodesToExplore, int32 StartX,
    int32 StartY, int32 GoalX, int32 GoalY, int32 GridSizeX)
{
    FPathNode& StartNode = PathNodes[AGridManager::StaticGetIndexFromXY(StartX, StartY, GridSizeX)];
    StartNode.CostFromStart = 0;
    StartNode.EstimatedCostToGoal = CalculateDistanceToGoal(StartX, StartY, GoalX, GoalY);
    NodesToExplore.Add(&StartNode);
}

PathFinder::FPathNode* PathFinder::FindNodeWithLowestCost(TArray<FPathNode*>& NodesToExplore)
{
    int32 CurrentIndex = 0;
    int32 LowestCost = MAX_int32;
    
    for (int32 i = 0; i < NodesToExplore.Num(); ++i)
    {
        const int32 Cost = NodesToExplore[i]->GetTotalCost();
        if (Cost < LowestCost)
        {
            LowestCost = Cost;
            CurrentIndex = i;
        }
    }
    
    // Remove the lowest cost node from the list
    FPathNode* Node = NodesToExplore[CurrentIndex];
    NodesToExplore.RemoveAtSwap(CurrentIndex);
    return Node;
}

bool PathFinder::IsGoalNode(const FPathNode* Node, int32 GoalX, int32 GoalY)
{
    return Node->X == GoalX && Node->Y == GoalY;
}

bool PathFinder::ProcessNeighbor(const TPair<int32, int32>& Direction, FPathNode* CurrentNode,
    TArray<FPathNode>& PathNodes, const TArray<FGridNode>& Grid, int32 GridSizeX, int32 GridSizeY, int32 GoalX,
    int32 GoalY, TArray<FPathNode*>& NodesToExplore)
{
    // Calculate neighbor coordinates
    const int32 NeighborX = CurrentNode->X + Direction.Key;
    const int32 NeighborY = CurrentNode->Y + Direction.Value;
    
    // Check if neighbor is valid and walkable
    if (!AGridManager::StaticIsValidPos(NeighborX, NeighborY, GridSizeX, GridSizeY) ||
        !IsNodeCrossable(Grid, GridSizeX, NeighborX, NeighborY))
    {
        return false;
    }
    
    FPathNode& NeighborNode = PathNodes[AGridManager::StaticGetIndexFromXY(NeighborX, NeighborY, GridSizeX)];
    
    if (NeighborNode.IsExplored)
    {
        return false;
    }
    
    // Calculate new cost to reach this neighbor
    const bool IsDiagonal = Direction.Key != 0 && Direction.Value != 0;
    const int32 MovementCost = IsDiagonal ? DIAGONAL_COST : STRAIGHT_COST;
    const int32 NewCostFromStart = CurrentNode->CostFromStart + MovementCost;
    
    // Update neighbor if we found a better path
    if (NeighborNode.CostFromStart == MAX_int32 || NewCostFromStart < NeighborNode.CostFromStart)
    {
        UpdateNeighborNode(NeighborNode, CurrentNode, NewCostFromStart, GoalX, GoalY, NodesToExplore);
        return true;
    }

    return false;
}

void PathFinder::UpdateNeighborNode(FPathNode& NeighborNode, FPathNode* CurrentNode, int32 NewCostFromStart,
    int32 GoalX, int32 GoalY, TArray<FPathNode*>& NodesToExplore)
{
    // Update node costs and path
    NeighborNode.CostFromStart = NewCostFromStart;
    NeighborNode.EstimatedCostToGoal = CalculateDistanceToGoal(NeighborNode.X, NeighborNode.Y, GoalX, GoalY);
    NeighborNode.PreviousNode = CurrentNode;
    
    bool IsInList = false;
    for (FPathNode* Node : NodesToExplore)
    {
        if (Node == &NeighborNode)
        {
            IsInList = true;
            break;
        }
    }
    
    if (!IsInList)
    {
        NodesToExplore.Add(&NeighborNode);
    }
}

int32 PathFinder::CalculateDistanceToGoal(int32 FromX, int32 FromY, int32 ToX, int32 ToY)
{
    // Manhattan distance heuristic > https://en.wikipedia.org/wiki/Taxicab_geometry
    const int32 DeltaX = FMath::Abs(ToX - FromX);
    const int32 DeltaY = FMath::Abs(ToY - FromY);
    return STRAIGHT_COST * (DeltaX + DeltaY);
}

bool PathFinder::IsNodeCrossable(const TArray<FGridNode>& Grid, int32 GridSizeX, int32 X, int32 Y)
{
    if (!AGridManager::StaticIsValidPos(X, Y, GridSizeX, GridSizeX))
    {
        return false;
    }
    
    const int32 Index = AGridManager::StaticGetIndexFromXY(X, Y, GridSizeX);
    return Grid[Index].IsCrossable;
}

TArray<FVector> PathFinder::ReconstructPathToStart(FPathNode* EndNode, float CellSize)
{
    TArray<FVector> Path;
    FPathNode* CurrentNode = EndNode;

    // Follow parent pointers back to start
    while (CurrentNode != nullptr)
    {
        const FVector WorldPosition(
            (CurrentNode->X + 0.5f) * CellSize,
            (CurrentNode->Y + 0.5f) * CellSize,
            0.0f
        );
        
        Path.Insert(WorldPosition, 0);
        CurrentNode = CurrentNode->PreviousNode;
    }

    return Path;
}