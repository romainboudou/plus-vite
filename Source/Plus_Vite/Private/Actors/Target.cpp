#include "Actors/Target.h"

#include "NavigationSystemTypes.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Game/Plus_ViteGameMode.h"
#include "Game/Plus_ViteProjectile.h"

ATarget::ATarget()
{
	PrimaryActorTick.bCanEverTick = true;

	TargetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TargetMesh"));
	RootComponent = TargetMesh;

	CenterHitSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CenterHitSphere"));
	CenterHitSphere->SetupAttachment(TargetMesh);
	CenterHitSphere->SetSphereRadius(20.f);
	CenterHitSphere->SetCollisionProfileName(TEXT("Trigger"));
	CenterHitSphere->OnComponentHit.AddDynamic(this, &ATarget::OnHit);

	SideHitSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SideHitSphere"));
	SideHitSphere->SetupAttachment(TargetMesh);
	SideHitSphere->SetSphereRadius(50.f);
	SideHitSphere->SetCollisionProfileName(TEXT("Trigger"));
	SideHitSphere->OnComponentHit.AddDynamic(this, &ATarget::OnHit);
}

void ATarget::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATarget::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	APlus_ViteProjectile* Projectile = Cast<APlus_ViteProjectile>(OtherActor);
	if (Projectile)
	{
		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, GetActorLocation());
		}
		
		APlus_ViteGameMode* GameMode = Cast<APlus_ViteGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			FString HitZone;
			
			if (HitComp == CenterHitSphere)
			{
				HitZone = "Center";
				GameMode->AddTime(15.f);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "HIT IN THE CENTER! Bonus Time!");
			}
			else if (HitComp == SideHitSphere)
			{
				HitZone = "Side";
				GameMode->AddTime(7.5f);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "HIT ON THE SIDE! Less Bonus Time!");
			}
			
			GameMode->AddDestroyedTarget();
			GameMode->UpdateLastShot(HitZone);
		}

		Destroy();
		Projectile->Destroy();
	}
}

