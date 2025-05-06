#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetSpawner.generated.h"

UCLASS()
class PLUS_VITE_API ATargetSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ATargetSpawner();
	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void SpawnTarget();

	UFUNCTION()
	void OnTargetDestroyed(AActor* DestoryedActor);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class ATarget> TargetClass;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TArray<AActor*> TargetPoints;

	ATarget* CurrentTarget;

	int32 LastSpawnIndex = -1;
	int32 SpawnCount = 0;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	int32 MawSpawns = 12;

};
