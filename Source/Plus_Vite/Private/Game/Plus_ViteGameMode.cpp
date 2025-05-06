// Copyright Epic Games, Inc. All Rights Reserved.

#include "Game/Plus_ViteGameMode.h"
#include "Characters/Plus_ViteCharacter.h"
#include "Components/TP_WeaponComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/TargetPoint.h"

APlus_ViteGameMode::APlus_ViteGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	PrimaryActorTick.bCanEverTick = true;
	
}

void APlus_ViteGameMode::BeginPlay()
{
	Super::BeginPlay();

	ShotResults.Init(FShotResult{ "Miss" }, 12);

	TArray<AActor*> AllTargetPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), AllTargetPoints);

	for (AActor* TargetPoint : AllTargetPoints)
	{
		if (TargetPoint->ActorHasTag("PlayerSpawn"))
		{
			PlayerSpawnPoints.Add(TargetPoint);
		}
	}

	SpawnPlayer();
}

void APlus_ViteGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GameTime -= DeltaTime;
	
	if (GameTime <= 0)
	{
		GameTime = 0;
		GetWorld()->GetTimerManager().ClearTimer(GameTimerHandle);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Temps écoulé ! Game Over"));
		ShowFinalScore();
	}

	int32 Secondes = FMath::FloorToInt(GameTime);
	int32 Millisecondes = FMath::FloorToInt((GameTime - Secondes) * 1000);

	GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Yellow,  
		FString::Printf(TEXT("Temps restant : %d.%03d"), Secondes, Millisecondes));
}

void APlus_ViteGameMode::AddTime(float TimeToAdd)
{
	GameTime += TimeToAdd;
}


void APlus_ViteGameMode::AddDestroyedTarget()
{
	DestroyedTargets++;
	GEngine->AddOnScreenDebugMessage(2, 2.0f, FColor::Green, FString::Printf(TEXT("Cibles détruites : %d"), DestroyedTargets));
}

void APlus_ViteGameMode::SpawnPlayer()
{
	if (PlayerSpawnPoints.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, PlayerSpawnPoints.Num() - 1);
		FVector SpawnLocation = PlayerSpawnPoints[RandomIndex]->GetActorLocation();
		FRotator SpawnRotation = PlayerSpawnPoints[RandomIndex]->GetActorRotation();

		AController* PlayerController = GetWorld()->GetFirstPlayerController();

		if (PlayerController)
		{
			APawn* PlayerPawn = PlayerController->GetPawn();

			if (PlayerPawn)
			{
				PlayerPawn->SetActorLocation(SpawnLocation);
				PlayerPawn->SetActorRotation(SpawnRotation);

				MoveObjectWithTag(TEXT("WeaponTag"), SpawnLocation + FVector(0.f, 150.f, -100.f), SpawnRotation);
			}
			
		}
	}
}

void APlus_ViteGameMode::RegisterShot(FString HitZone)
{
	ShotResults[CurrentShotIndex].HitZone = HitZone;

	CurrentShotIndex = (CurrentShotIndex + 1) % 12;
	
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Tir enregistré : %s"), *HitZone));
}

void APlus_ViteGameMode::UpdateLastShot(FString HitZone)
{
	int32 LastIndex = (CurrentShotIndex - 1 + 12) % 12;
	ShotResults[LastIndex].HitZone = HitZone;
}


void APlus_ViteGameMode::ShowFinalScore()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("TEST"));
	for (const FShotResult& Result : ShotResults)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Tir : %s"), *Result.HitZone));
	}
}

void APlus_ViteGameMode::MoveObjectWithTag(FName Tag, FVector NewLocation, FRotator NewRotation)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag, FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (Actor)
		{
			Actor->SetActorLocation(NewLocation);
			Actor->SetActorRotation(NewRotation);
		}
	}
}



