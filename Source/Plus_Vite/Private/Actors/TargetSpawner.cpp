#include "Actors/TargetSpawner.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h"
#include "Actors/Target.h"

ATargetSpawner::ATargetSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATargetSpawner::BeginPlay()
{
	Super::BeginPlay();
		
	TArray<AActor*> AllTargetPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), AllTargetPoints);

	for (AActor* TargetPoint : AllTargetPoints)
	{
		if (TargetPoint->ActorHasTag("TargetSpawn"))
		{
			TargetPoints.Add(TargetPoint);
		}
	}
	
	SpawnTarget();
	
}

void ATargetSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATargetSpawner::SpawnTarget()
{
	if (SpawnCount >= MawSpawns || TargetPoints.Num() == 0 || !TargetClass)
	{
		return;
	}
	
	int32 RandomIndex;

	do
	{
		RandomIndex = FMath::RandRange(0, TargetPoints.Num() -1);
	}
	while (RandomIndex == LastSpawnIndex && TargetPoints.Num() > 1);
	
	LastSpawnIndex = RandomIndex;
	FVector SpawnLocation = TargetPoints[RandomIndex]->GetActorLocation();
	FRotator SpawnRotation = TargetPoints[RandomIndex]->GetActorRotation();

	CurrentTarget = GetWorld()->SpawnActor<ATarget>(TargetClass, SpawnLocation, SpawnRotation);

	if (CurrentTarget)
	{
		CurrentTarget->OnDestroyed.AddDynamic(this, &ATargetSpawner::OnTargetDestroyed);
	}

	SpawnCount++;
	GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Purple, FString::Printf(TEXT("Cibles : %d"), SpawnCount));
}

void ATargetSpawner::OnTargetDestroyed(AActor* DestoryedActor)
{
	SpawnTarget();
}



