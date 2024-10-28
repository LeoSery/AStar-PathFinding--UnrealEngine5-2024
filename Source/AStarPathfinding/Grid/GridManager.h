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
	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	int32 GridSizeX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	int32 GridSizeY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	float GridCellSize;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TArray<FGridNode> Grid;

	int32 GetIndexFromXY(int32 X, int32 Y) const;
	bool IsValidPos(int32 X, int32 Y) const;
	FGridNode& GetNode(int32 X, int32 Y);

	void Initialize();
	void DrawGrid();

	FVector GridOrigin;
};
