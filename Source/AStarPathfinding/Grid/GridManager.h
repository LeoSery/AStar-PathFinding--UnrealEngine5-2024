#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridNode.h"
#include "GridManager.generated.h"

UCLASS()
class ASTARPATHFINDING_API AGridManager : public AActor
{
	GENERATED_BODY()

public:
	AGridManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	int32 GridSizeX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	int32 GridSizeY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	float CellSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Settings")
	float InteractionDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Settings")
	float InteractionRate;
	
	UFUNCTION(BlueprintCallable, Category = "Grid")
	bool GetCellFromWorldPosition(const FVector& WorldPosition, int32& OutX, int32& OutY) const;
	
	UFUNCTION(BlueprintCallable, Category = "Grid")
	FVector GetWorldPositionFromCell(int32 X, int32 Y) const;
	
	UFUNCTION(BlueprintCallable, Category = "Grid Debug")
	void DebugDrawCell(int32 X, int32 Y, FColor Color = FColor::Green, float Duration = 1.0f);
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
private:
	static constexpr float DEFAULT_CELL_SIZE = 100.0f;
	static constexpr int32 DEFAULT_GRID_SIZE = 10;

	UPROPERTY()
	TArray<FGridNode> Grid;

	int32 GetIndexFromXY(int32 X, int32 Y) const;
	bool IsValidPos(int32 X, int32 Y) const;
	FGridNode& GetNode(int32 X, int32 Y);

	void Initialize();
	void DrawGrid();
	void ClearDebugLines();

	int32 LastHighlightedNodeX;
	int32 LastHighlightedNodeY;
	bool bHasHighlightedNode;
	
	bool IsNodeAlreadyHighlighted(int32 X, int32 Y) const;
	void UpdateHighlightedCell(int32 X, int32 Y);
	
	FVector GridOrigin;
};