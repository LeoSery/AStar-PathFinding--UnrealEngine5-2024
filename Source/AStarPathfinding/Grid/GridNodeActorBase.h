#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridNodeActorBase.generated.h"

UENUM(BlueprintType)
enum class EGridActorType : uint8
{
	Start   UMETA(DisplayName = "Start Point"),
	Goal    UMETA(DisplayName = "Goal Point"),
	Wall    UMETA(DisplayName = "Wall"),
	None    UMETA(DisplayName = "None")
};

UENUM(BlueprintType)
enum class ENodeState : uint8
{
	Default     UMETA(DisplayName = "Default"),
	ToExplore   UMETA(DisplayName = "To Explore"),
	Explored    UMETA(DisplayName = "Explored"),
	Path        UMETA(DisplayName = "Path"),
	Neighbor    UMETA(DisplayName = "Neighbor")
};

UCLASS(Abstract)
class ASTARPATHFINDING_API AGridNodeActorBase : public AActor
{
	GENERATED_BODY()
	
public:
	//////// CONSTRUCTOR ////////
	AGridNodeActorBase();

	//////// FIELDS ////////
	//// Graphic fields
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComponent;

	//// Grid fields
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid")
	EGridActorType NodeType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	ENodeState NodeState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	int32 GridX;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	int32 GridY;

	//////// METHODS ////////
	UFUNCTION(BlueprintCallable, Category = "Grid")
	void SetupNodeColor(EGridActorType Type, ENodeState State = ENodeState::Default);

	void UpdatePathFindingNodeColor(ENodeState State);
	
protected:
	//////// UNREAL LIFECYCLE ////////
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
private:
	UPROPERTY()
	bool bIsBaseColorSet = false;

	void LockBaseColor() { bIsBaseColorSet = true; }
	bool IsBaseColorSet() const { return bIsBaseColorSet; }
};
