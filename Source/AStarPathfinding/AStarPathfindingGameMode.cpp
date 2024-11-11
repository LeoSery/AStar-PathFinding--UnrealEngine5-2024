// Copyright Epic Games, Inc. All Rights Reserved.

#include "AStarPathfindingGameMode.h"
#include "AStarPathfindingPlayerController.h"
#include "AStarPathfindingCharacter.h"
#include "GameFramework/SpectatorPawn.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AAStarPathfindingGameMode::AAStarPathfindingGameMode()
{
	// Try to find and assign the custom camera pawn
	static ConstructorHelpers::FClassFinder<APawn> GridCameraPawnClass(TEXT("/Game/TopDown/Blueprints/BP_GridFreeCamera"));
	if (GridCameraPawnClass.Class != nullptr)
	{
		DefaultPawnClass = GridCameraPawnClass.Class;
		UE_LOG(LogTemp, Warning, TEXT("BP_GridCamera found and assigned"));
	}
	else
	{
		// Fallback to default spectator pawn if custom camera not found
		UE_LOG(LogTemp, Error, TEXT("Failed to find BP_GridCamera"));
		DefaultPawnClass = ASpectatorPawn::StaticClass();
	}
	
	PlayerControllerClass = AAStarPathfindingPlayerController::StaticClass();

	// Try to find and assign the custom player controller blueprint
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

	// Try to find and assing GridManager blueprint
	static ConstructorHelpers::FClassFinder<AGridManager> GridManagerBPClass(TEXT("/Game/TopDown/Blueprints/Grid/BP_GridManager"));
	if(GridManagerBPClass.Succeeded())
	{
		GridManagerClass = GridManagerBPClass.Class;
		UE_LOG(LogTemp, Warning, TEXT("BP_GridManager found"));
	}
	else 
	{
		GridManagerClass = AGridManager::StaticClass();
		UE_LOG(LogTemp, Warning, TEXT("BP_GridManager not found, using C++ class"));
	}
}

void AAStarPathfindingGameMode::BeginPlay()
{
	Super::BeginPlay();
	InitializeGrid();
}

AActor* AAStarPathfindingGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	AActor* PlayerStart = Super::ChoosePlayerStart_Implementation(Player);

	if (PlayerStart)
	{
		FVector Location = PlayerStart->GetActorLocation();
		Location.Z += 1000.0f;
		PlayerStart->SetActorLocation(Location);
        
		FRotator Rotation(300.0f, 0.0f, 0.0f);
		PlayerStart->SetActorRotation(Rotation);
	}
    
	return PlayerStart;
}

void AAStarPathfindingGameMode::OnGridStateChanged()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Grid state changed"));
}

void AAStarPathfindingGameMode::OnPathReCalculated(const TArray<FVector>& Path, const TArray<FVector>& ExploredNodes)
{
	FString Message = FString::Printf(TEXT("New path calculated with %d points, %d nodes explored"), Path.Num(), ExploredNodes.Num());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, Message);
}

void AAStarPathfindingGameMode::InitializeGrid()
{
	TArray<AActor*> FoundGridManagers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridManager::StaticClass(), FoundGridManagers);

	if (FoundGridManagers.Num() > 0)
	{
		GridManager = Cast<AGridManager>(FoundGridManagers[0]);
		
		if (FoundGridManagers.Num() > 1)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, 
				FString::Printf(TEXT("Multiple GridManagers found. Keeping first one, destroying %d others."), 
				FoundGridManagers.Num() - 1));
			
			for (int32 i = 1; i < FoundGridManagers.Num(); i++)
			{
				if (FoundGridManagers[i])
				{
					FoundGridManagers[i]->Destroy();
				}
			}
		}
	}
	else
	{
		if (!GridManagerClass)
		{
			return;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        
		GridManager = GetWorld()->SpawnActor<AGridManager>(GridManagerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	}

	if (GridManager)
	{
		GridManager->OnGridChanged.AddDynamic(this, &AAStarPathfindingGameMode::OnGridStateChanged);
		GridManager->OnPathUpdated.AddDynamic(this, &AAStarPathfindingGameMode::OnPathReCalculated);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Grid Manager initialized successfully"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to initialize Grid Manager"));
	}
}

