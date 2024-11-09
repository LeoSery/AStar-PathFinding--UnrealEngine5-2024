#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridNode.h"
#include "GridNodeActorBase.h"
#include "GridManager.generated.h"

UCLASS()
class ASTARPATHFINDING_API AGridManager : public AActor
{
	GENERATED_BODY()

public:
	//////// CONSTRUCTOR ////////
	AGridManager();

	//////// FIELDS ////////
	//// Grid fields
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Settings")
	int32 GridSizeX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Settings")
	int32 GridSizeY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Settings")
	float CellSize;

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
	UFUNCTION(BlueprintCallable, Category = "Grid Debug")
	void DebugDrawCell(int32 X, int32 Y, FColor Color = FColor::Green, float Duration = 1.0f);
	UFUNCTION(BlueprintCallable, Category = "Grid")
	FVector GetHighlightedCellWorldPosition() const;

	//// Nodes methods
	UFUNCTION(BlueprintCallable, Category = "Grid|Interaction")
	bool PlaceNodeActorInGrid(const FVector& WorldPosition);
	UFUNCTION(BlueprintCallable, Category = "Grid|Interaction")
	bool RemoveNodeActorFromGrid(const FVector& WorldPosition);
	
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

	//////// METHODS ////////
	///Grid methods
	void Initialize();
	void DrawGrid();
	void ClearDebugLines();

	int32 GetIndexFromXY(int32 X, int32 Y) const;
	bool IsValidPos(int32 X, int32 Y) const;
	FGridNode& GetNode(int32 X, int32 Y);
	bool IsNodeAlreadyHighlighted(int32 X, int32 Y) const;
	void UpdateHighlightedCell(int32 X, int32 Y);

	//// Nodes methods
	void RemoveExistingNodeActorAtCell(int32 X, int32 Y);
	AGridNodeActorBase* SpawnNodeActor(TSubclassOf<AGridNodeActorBase> ActorClass, int32 X, int32 Y);
};
