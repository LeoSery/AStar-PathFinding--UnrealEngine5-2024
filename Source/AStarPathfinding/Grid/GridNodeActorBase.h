#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridNodeActorBase.generated.h"

UENUM(BlueprintType)
enum class EGridActorType : uint8
{
	Start   UMETA(DisplayName = "Start Point"),
	Goal    UMETA(DisplayName = "Goal Point"),
	Wall    UMETA(DisplayName = "Wall")
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
	int32 GridX;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	int32 GridY;

	//////// METHODS ////////
	UFUNCTION(BlueprintCallable, Category = "Grid")
	void SetupNodeColor(EGridActorType Type);
	
protected:
	//////// UNREAL LIFECYCLE ////////
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
