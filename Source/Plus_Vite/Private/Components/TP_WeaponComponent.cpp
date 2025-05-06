// Copyright Epic Games, Inc. All Rights Reserved.


#include "Components/TP_WeaponComponent.h"
#include "Characters/Plus_ViteCharacter.h"
#include "Game/Plus_ViteProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Game/Plus_ViteGameMode.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}


void UTP_WeaponComponent::Fire()
{
    if (Character == nullptr || Character->GetController() == nullptr)
    {
        return;
    }

    if (!CanShoot())
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Plus de munitions !"));
        return;
    }

    APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()); // Déclaré une seule fois
    APlus_ViteProjectile* SpawnedProjectile = nullptr; // Déclaré avant utilisation

    if (ProjectileClass != nullptr)
    {
        UWorld* const World = GetWorld();
        if (World != nullptr)
        {
            const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
            const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

            FActorSpawnParameters ActorSpawnParams;
            ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

            SpawnedProjectile = World->SpawnActor<APlus_ViteProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

            AmmoCount--;

            if (AmmoCount == 0)
            {
                UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.2f);
                FTimerHandle TimerHandle;
                GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UTP_WeaponComponent::ResetTimeDilation, 3.0f, false);
            }

        	if (SpawnedProjectile)
        	{
        		APlus_ViteGameMode* GameMode = Cast<APlus_ViteGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
        		if (GameMode)
        		{
        			GameMode->RegisterShot("Miss");  
        		}
        	}

            if (SpawnedProjectile && AmmoCount == 0)
            {
                if (PlayerController)
                {
                    PlayerController->SetViewTargetWithBlend(SpawnedProjectile, 0.2f);

                    FTimerHandle CameraResetHandle;
                    GetWorld()->GetTimerManager().SetTimer(CameraResetHandle, this, &UTP_WeaponComponent::ResetCamera, 1.0f, false);
                }
            }

            GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::Printf(TEXT("Munitions restantes : %d"), AmmoCount));
        }
    }

    if (FireSound != nullptr)
    {
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
    }

    if (FireAnimation != nullptr)
    {
        UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
        if (AnimInstance != nullptr)
        {
            AnimInstance->Montage_Play(FireAnimation, 1.f);
        }
    }
}



bool UTP_WeaponComponent::AttachWeapon(APlus_ViteCharacter* TargetCharacter)
{
	Character = TargetCharacter;

	// Check that the character is valid, and has no weapon component yet
	if (Character == nullptr || Character->GetInstanceComponents().FindItemByClass<UTP_WeaponComponent>())
	{
		return false;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	// add the weapon as an instance component to the character
	Character->AddInstanceComponent(this);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);
		}
	}

	return true;
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}

bool UTP_WeaponComponent::CanShoot() const
{
	return AmmoCount > 0;
}

void UTP_WeaponComponent::ResetTimeDilation()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
}

void UTP_WeaponComponent::ResetCamera()
{
	if (Character && Character->GetController())
	{
		APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
		if (PlayerController)
		{
			PlayerController->SetViewTargetWithBlend(Character, 0.5f);
		}
	}
}

