// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Plus_ViteGameMode.generated.h"
USTRUCT(BlueprintType)
struct FShotResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString HitZone; 
};
UCLASS(minimalapi)
class APlus_ViteGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APlus_ViteGameMode();

	void AddDestroyedTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float GameTime = 60.f;


	UFUNCTION(BlueprintCallable, Category = "GameMode")
	void AddTime(float TimeToAdd);

	UFUNCTION()
	void RegisterShot(FString HitZone);

	UFUNCTION()
	void UpdateLastShot(FString HitZone);

	UFUNCTION()
	void ShowFinalScore();

	UFUNCTION()
	void MoveObjectWithTag(FName Tag, FVector NewLocation, FRotator NewRotation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FShotResult> ShotResults;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void SpawnPlayer();

	UPROPERTY()
	TArray<AActor*> PlayerSpawnPoints;

private:
	FTimerHandle GameTimerHandle;

	int32 DestroyedTargets = 0;

	int32 CurrentShotIndex = 0;

};






