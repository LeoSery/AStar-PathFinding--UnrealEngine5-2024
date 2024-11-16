# AStar-PathFinding--UnrealEngine5-2024

This project is a student project implementing the ***A* Pathfinding Algorithm***, realized in **C++ with Unreal Engine 5** for my Master's 2 degree in game development. It's an interactive visualization where users can create **obstacles**, place **start** and **end** points, and observe the algorithm **finding the optimal path in real-time**. The project focuses on core A* implementation and basic **visualization**.

## Summary

- [Project pitch](#project-pitch)
- [Technical Section](#technical-section)
- [How to play the demo](#how-to-play-the-demo)
- [Possible improvements](#possible-improvements)

## Project pitch

This project implements an **interactive A* pathfinding visualization** using Unreal Engine 5 and C++.

More about "*A* Pathfinding Algorithm*" [Here](https://en.wikipedia.org/wiki/A*_search_algorithm)

**Features included:**
- Interactive grid with debug visualization
- Real-time pathfinding calculation
- Dynamic obstacle system
- Free camera to explore the visualization
- Real-time node placement via highlight system
- Optimized path calculation
- Visual feedback of explored nodes and final path

**Controls:**
*Movement:*
- Forward > `Z`
- Left > `Q`
- Backward > `S`
- Right > `D`
- Up > `Space`
- Down > `Left Shift`

*Camera:*
- Hold Right Click + Mouse > Rotate Camera
- Mouse > Select cell with highlight

*Node Placement:*
- `Double Right-Click` > Place/Remove selected node type
- `1` > Select ***StartNode***
- `2` > Select ***EndNode***
- `3` > Select ***WallNode***

## Technical Section

### 1. Node System: Visual & Logic Components

The project separates visual and logical node management for better organization and performance.

#### Visual Representation (Actors)

The [`GridNodeActorBase`](/Source/AStarPathfinding/Grid/GridNodeActorBase.h) class serves as the base for all visual node representations:

```cpp
UCLASS(Abstract)
class AGridNodeActorBase : public AActor
{
    GENERATED_BODY()
    
public:
    UPROPERTY(EditDefaultsOnly, Category = "Grid")
    EGridActorType NodeType;

    UPROPERTY(VisibleAnywhere, Category = "Grid")
    int32 GridX, GridY;
    
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void SetupNodeColor(EGridActorType Type, ENodeState State = ENodeState::Default);
};
```

This base class spawns visual representations when placing nodes on the grid. It's inherited by:
- [`StartNodeActor`](/Source/AStarPathfinding/Grid/StartNodeActor.h) > Represents the pathfinding start point
- [`GoalNodeActor`](/Source/AStarPathfinding/Grid/GoalNodeActor.h) > Represents the pathfinding goal
- [`WallNodeActor`](/Source/AStarPathfinding/Grid/WallNodeActor.h) > Represents obstacles

#### Logical Grid Structure

The [`GridNode`](/Source/AStarPathfinding/Grid/GridNode.h) structure handles the *logical state* :

```cpp
USTRUCT(BlueprintType)
struct FGridNode
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector WorldPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool IsCrossable;
};
```

This structure handles the logical state of each grid position, *independent of visual representation*.

### 2. Grid Management System

The [`GridManager`](/Source/AStarPathfinding/Grid/GridManager.h) handles grid visualization and coordinate management:

```cpp
void AGridManager::DrawGrid()
{
    if (!GetWorld())
    {
        return;
    }

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
```

Key Helper Methods:
- `GetWorldPositionFromCell(X, Y)`: Converts grid coordinates to world space
- `GetCellFromWorldPosition(WorldPosition)`: Converts world space to grid coordinates
- `IsValidPos(X, Y)`: Validates grid coordinates
- `GetIndexFromXY(X, Y)`: Converts 2D coordinates to array index
- `DebugDrawCell(X, Y, Color)`: Highlights specific cells for debugging

### 3. A* Pathfinding Implementation

The [`PathFinder`](/Source/AStarPathfinding/Solver/PathFinder.cpp) class implements the A* algorithm. Let's break down the main components:

#### Main Algorithm
```cpp
static TArray<FVector> PathFinder::Compute(/* ... */)
{
    // Initial validation
    if (!ValidateInputs(StartX, StartY, GoalX, GoalY, GridSizeX, GridSizeY))
    {
        return TArray<FVector>();
    }
    
    // Setup phase
    TArray<FPathNode> PathNodes;
    InitializePathNodes(PathNodes, GridSizeX, GridSizeY);
    TArray<FPathNode*> NodesToExplore;
    SetupStartNode(PathNodes, NodesToExplore, StartX, StartY, GoalX, GoalY, GridSizeX);
```
This initial phase validates inputs and prepares the data structures needed for pathfinding.

```cpp
    // Main exploration loop
    while (!NodesToExplore.IsEmpty())
    {
        FPathNode* CurrentNode = FindNodeWithLowestCost(NodesToExplore);
        
        if (IsGoalNode(CurrentNode, GoalX, GoalY))
        {
            return ReconstructPathToStart(CurrentNode, CellSize);
        }
        
        CurrentNode->IsExplored = true;
```
The main loop continuously explores nodes, starting with the most promising one (lowest F cost).

```cpp
        // Neighbor exploration
        for (const auto& Direction : Directions)
        {
            ProcessNeighbor(Direction, CurrentNode, PathNodes, Grid,
                GridSizeX, GridSizeY, GoalX, GoalY, NodesToExplore);
        }
    }
```
For each **node**, we examine all possible **neighbors** and update their **costs**.

Key Helper Methods:
- `FindNodeWithLowestCost()`: Selects the next node to explore based on *F* cost (*G* + *H*)
- `ProcessNeighbor()`: Evaluates and updates a neighboring node's costs and state
- `ReconstructPathToStart()`: Traces back through parent nodes to build the final path
- `CalculateDistanceToGoal()`: Computes heuristic cost using Manhattan distance
- `IsNodeCrossable()`: Validates if a node can be traversed

Core Components:
- Cost Constants: `STRAIGHT_COST` *(10)*, `DIAGONAL_COST` *(14)*
- Direction Management: Pre-computed array of 8 neighbor offsets
- Node Validation: Grid boundary and obstacle checking
- Path Reconstruction: Parent node backtracking system

This implementation balances efficiency with readability, using helper methods to break down complex operations while maintaining performance through careful data structure choice and cost calculations.

## How to play the demo

1. Clone the repository
2. Open the project in Unreal Engine 5.4 or later
3. Navigate to `Content > TopDown > Blueprints > Grid > BP_GridManager` in the Content Browser
4. In the Details panel, under the Grid Settings section, you can configure:
   - Grid Size X: `20` *(default)*
   - Grid Size Y: `20` *(default)*
   - Cell Size: `100.0` *(default)*
5. Click Play in the editor
6. [Use the controls](#project-pitch) described above to:
   - Place `StartNode` *(green)* and `GoalNode` *(blue)* points
   - Create `WallNodes` *(grey)*
   - Watch the pathfinding algorithm work in real-time
   - Use the free camera to explore from different angles

## Possible improvements

This project provides a functional base for the implementation and viusalization of the A* algorithm using Unreal Engine 5 and C++. As a study project, it focuses on providing a solid implementation of the basic algorithm while maintaining a clear, easy-to-maintain code base. The visualization provides real-time interaction and demonstration of the path, serving as a basis for possible future improvements :

1. **User Interface Enhancements:**
   - Add an in-game UI panel for real-time grid and nodes configuration
   - Implement visual feedback for selected node types
   - Display pathfinding statistics (path length, nodes explored and nodes cost)

2. **Feedback System:**
   - Replace debug screen logs with proper UI notifications
   - Add visual effects for node state changes
   - Implement a more detailed visualization of the pathfinding process
   - Show cost calculations in real-time for educational purposes

3. **Performance Optimization:**
   - Memory Management:
     - Implement object pooling for nodes instead of frequent creation/destruction
     - Use Instanced Static Mesh components for node visualization instead of individual StaticMeshComponents, significantly reducing draw calls and memory usage

   - Computation Optimization:
     - Implement partial path recalculation by only updating affected areas when obstacles change
     - Consider caching previously calculated paths for similar start/end configurations

   - Threading:
     - Move pathfinding calculations to a separate thread to prevent blocking the main game thread
     - Implement asynchronous visual updates for better performance during path calculation
     - Consider using a job system for parallel node evaluation
