// PathFinder.h
#pragma once

#include "CoreMinimal.h"
#include "AStarPathfinding/Grid/GridNode.h"

class ASTARPATHFINDING_API PathFinder
{
public:
    //////// CONSTANTS ////////
    /// costs constants
    static constexpr int32 STRAIGHT_COST = 10;
    static constexpr int32 DIAGONAL_COST = 14;
    
    //////// STRUCTS ////////
    /// nodes structs
    struct FPathNode
    {
        int32 X;
        int32 Y;
        int32 CostFromStart;
        int32 EstimatedCostToGoal;
        FPathNode* PreviousNode;
        bool IsExplored;
    
        int32 GetTotalCost() const
        {
            return CostFromStart + EstimatedCostToGoal;
        }
    };

    //////// METHODS ////////
    /// main method
    static TArray<FVector> Compute(
        const TArray<FGridNode>& Grid,
        int32 GridSizeX,
        int32 GridSizeY,
        int32 StartX,
        int32 StartY,
        int32 GoalX,
        int32 GoalY,
        float CellSize
    );

private:
    //////// FIELDS ////////
    /// helper fields
    static const TArray<TPair<int32, int32>> Directions;

    //////// METHODS ////////
    /// Initialization methods
    static bool ValidateInputs(int32 StartX, int32 StartY, int32 GoalX, int32 GoalY, int32 GridSizeX, int32 GridSizeY);
    static void InitializePathNodes(TArray<FPathNode>& PathNodes, int32 GridSizeX, int32 GridSizeY);
    static void InitializeNode(FPathNode& Node, int32 X, int32 Y);
    static void SetupStartNode(
        TArray<FPathNode>& PathNodes,
        TArray<FPathNode*>& NodesToExplore,
        int32 StartX,
        int32 StartY,
        int32 GoalX,
        int32 GoalY,
        int32 GridSizeX
    );

    /// Pathfinding methods
    static FPathNode* FindNodeWithLowestCost(TArray<FPathNode*>& NodesToExplore);
    static bool IsGoalNode(const FPathNode* Node, int32 GoalX, int32 GoalY);
    static bool ProcessNeighbor(
        const TPair<int32, int32>& Direction,
        FPathNode* CurrentNode,
        TArray<FPathNode>& PathNodes,
        const TArray<FGridNode>& Grid,
        int32 GridSizeX,
        int32 GridSizeY,
        int32 GoalX,
        int32 GoalY,
        TArray<FPathNode*>& NodesToExplore
    );
    
    static void UpdateNeighborNode(
        FPathNode& NeighborNode,
        FPathNode* CurrentNode,
        int32 NewCostFromStart,
        int32 GoalX,
        int32 GoalY,
        TArray<FPathNode*>& NodesToExplore
    );
    
    /// Helpers methods
    static int32 CalculateDistanceToGoal(int32 FromX, int32 FromY, int32 ToX, int32 ToY);
    static bool IsNodeCrossable(const TArray<FGridNode>& Grid, int32 GridSizeX, int32 X, int32 Y);
    static TArray<FVector> ReconstructPathToStart(FPathNode* EndNode, float CellSize);
};