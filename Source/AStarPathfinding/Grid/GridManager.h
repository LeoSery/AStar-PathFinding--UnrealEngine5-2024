#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridNode.h"
#include "GridNodeActorBase.h"
#include "GridManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGridChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPathUpdated, const TArray<FVector>&, Path, const TArray<FVector>&, ExploredNodes);

UCLASS()
class ASTARPATHFINDING_API AGridManager : public AActor
{
	GENERATED_BODY()

public:
	//////// CONSTRUCTOR ////////
	AGridManager();

	//////// STATIC METHODS ////////
	static bool StaticIsValidPos(int32 X, int32 Y, int32 GridSizeX, int32 GridSizeY);
	static int32 StaticGetIndexFromXY(int32 X, int32 Y, int32 GridSizeX);

	//////// DELEGATES ////////
	//// Grid delegates
	UPROPERTY(BlueprintAssignable, Category = "Grid|Events")
	FOnGridChanged OnGridChanged;
    
	UPROPERTY(BlueprintAssignable, Category = "Grid|Events")
	FOnPathUpdated OnPathUpdated;
	
	//////// FIELDS ////////
	//// Grid fields
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Settings")
	int32 GridSizeX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Settings")
	int32 GridSizeY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Settings")
	float CellSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid|Components")
	UStaticMeshComponent* FloorMesh;

	//// Interaction fields
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Settings")
	float InteractionDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Settings")
	float InteractionRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Settings")
	EGridActorType CurrentPlacementType;
	
	//// Nodes fields
	UPROPERTY(EditDefaultsOnly, Category = "Grid|Nodes")
	TSubclassOf<AGridNodeActorBase> StartNodeClass;
	UPROPERTY(EditDefaultsOnly, Category = "Grid|Nodes")
	TSubclassOf<AGridNodeActorBase> GoalNodeClass;
	UPROPERTY(EditDefaultsOnly, Category = "Grid|Nodes")
	TSubclassOf<AGridNodeActorBase> WallNodeClass;

	//////// METHODS ////////
	//// Grid methods
	UFUNCTION(BlueprintCallable, Category = "Grid")
	FVector GetWorldPositionFromCell(int32 X, int32 Y) const;
	UFUNCTION(BlueprintCallable, Category = "Grid")
	bool GetCellFromWorldPosition(const FVector& WorldPosition, int32& OutX, int32& OutY) const;
	UFUNCTION(BlueprintCallable, Category = "Grid|Debug")
	void DebugDrawCell(int32 X, int32 Y, FColor Color = FColor::Green, float Duration = 1.0f);
	UFUNCTION(BlueprintCallable, Category = "Grid")
	FVector GetHighlightedCellWorldPosition() const;
	UFUNCTION(BlueprintCallable, Category = "Grid")
	EGridActorType GetNodeTypeAtPosition(const FVector& WorldPosition) const;
	UFUNCTION(BlueprintCallable, Category = "Grid")
	void UpdateFloorMesh();
	
	//// Nodes methods
	UFUNCTION(BlueprintCallable, Category = "Grid|Interaction")
	bool ToggleNodeActorInGrid(const FVector& WorldPosition);
	
protected:
	//////// UNREAL LIFECYCLE ////////
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
private:
	//////// FIELDS ////////
	//// Grid fields
	static constexpr float DEFAULT_CELL_SIZE = 100.0f;
	static constexpr int32 DEFAULT_GRID_SIZE = 10;
	
	UPROPERTY()
	TArray<FGridNode> Grid;
	FVector GridOrigin;
	
	int32 LastHighlightedNodeX;
	int32 LastHighlightedNodeY;
	bool bHasHighlightedNode;

	//// Nodes fields
	UPROPERTY()
	AGridNodeActorBase* StartNode;
	UPROPERTY()
	AGridNodeActorBase* GoalNode;
	
	TMap<FIntPoint, AGridNodeActorBase*> WallNodes;

	//// Pathfinding fields
	TArray<FVector> CurrentPath;
	TArray<FVector> ExploredNodes;

	//////// METHODS ////////
	///Grid methods
	void Initialize();
	void DrawGrid();
	void ClearDebugLines();

	/// Grid helper
	int32 GetIndexFromXY(int32 X, int32 Y) const;
	bool IsValidPos(int32 X, int32 Y) const;
	FGridNode& GetNode(int32 X, int32 Y);
	bool IsNodeAlreadyHighlighted(int32 X, int32 Y) const;
	void UpdateHighlightedCell(int32 X, int32 Y);
	AGridNodeActorBase* GetNodeActorAtCell(int32 X, int32 Y) const;

	//// Nodes methods
	void RemoveExistingNodeActorAtCell(int32 X, int32 Y);
	AGridNodeActorBase* SpawnNodeActor(TSubclassOf<AGridNodeActorBase> ActorClass, int32 X, int32 Y);

	//// Pathfinding methods
	void UpdatePathfinding();
	void UpdateNodeStates();
	void ClearAllNodeStates();
};
