// Copyright Epic Games, Inc. All Rights Reserved.

#include "AStarPathfindingGameMode.h"
#include "AStarPathfindingPlayerController.h"
#include "AStarPathfindingCharacter.h"
#include "GameFramework/SpectatorPawn.h"
#include "UObject/ConstructorHelpers.h"

AAStarPathfindingGameMode::AAStarPathfindingGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> GridCameraPawnClass(TEXT("/Game/TopDown/Blueprints/BP_GridFreeCamera"));
	if (GridCameraPawnClass.Class != nullptr)
	{
		DefaultPawnClass = GridCameraPawnClass.Class;
		UE_LOG(LogTemp, Warning, TEXT("BP_GridCamera found and assigned"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find BP_GridCamera"));
		DefaultPawnClass = ASpectatorPawn::StaticClass();
	}
	
	PlayerControllerClass = AAStarPathfindingPlayerController::StaticClass();
    
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
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

