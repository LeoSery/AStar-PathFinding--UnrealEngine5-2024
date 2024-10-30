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

	FVector GridOrigin;
};