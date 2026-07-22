#include "Character/MultiplayerFPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Weapons/Weapon.h"
#include "InputMappingContext.h"
#include "Systems/AnimationSystem.h"
#include "Game/MultiplayerFPSGameMode.h"
#include "Net/UnrealNetwork.h"

AMultiplayerFPSCharacter::AMultiplayerFPSCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Don't rotate character with camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MaxAcceleration = 2048.0f;

	// Create camera component
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(RootComponent);
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 68.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Initialize health
	CurrentHealth = MaxHealth;

	// Initialize camera FOV
	CurrentFOV = DefaultFOV;
	if (FirstPersonCameraComponent)
	{
		FirstPersonCameraComponent->SetFieldOfView(CurrentFOV);
	}

	// Enable replication
	bReplicates = true;
	SetReplicateMovement(true);
}

void AMultiplayerFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			PlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Equip default weapon
	if (DefaultWeaponClass)
	{
		EquipWeapon(DefaultWeaponClass);
	}
}

void AMultiplayerFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCameraFOV(DeltaTime);

	// Update movement animation
	float CurrentSpeed = GetVelocity().Size();
	UAnimationSystem::UpdateMovementAnimation(GetMesh(), CurrentSpeed, bIsAiming, bIsSprinting);
}

void AMultiplayerFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent =
		FindComponentByClass<UEnhancedInputComponent>())
	{
		// Moving
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMultiplayerFPSCharacter::Move);
		}

		// Looking
		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMultiplayerFPSCharacter::Look);
		}

		// Jumping
		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		}

		// Sprinting
		if (SprintAction)
		{
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AMultiplayerFPSCharacter::StartSprint);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMultiplayerFPSCharacter::StopSprint);
		}

		// Aiming
		if (AimAction)
		{
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AMultiplayerFPSCharacter::StartAim);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AMultiplayerFPSCharacter::StopAim);
		}

		// Firing
		if (FireAction)
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AMultiplayerFPSCharacter::Fire);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AMultiplayerFPSCharacter::StopFiring);
		}

		// Reloading
		if (ReloadAction)
		{
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AMultiplayerFPSCharacter::Reload);
		}
	}
}

void AMultiplayerFPSCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (!MovementVector.IsZero())
	{
		// Move forward/backward
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);

		// Move left/right
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AMultiplayerFPSCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (!LookAxisVector.IsZero())
	{
		// Look up/down
		AddControllerPitchInput(LookAxisVector.Y);

		// Look left/right
		AddControllerYawInput(LookAxisVector.X);
	}
}

void AMultiplayerFPSCharacter::StartSprint()
{
	if (HasAuthority())
	{
		bIsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
	else
	{
		Server_SetSprinting(true);
	}
}

void AMultiplayerFPSCharacter::StopSprint()
{
	if (HasAuthority())
	{
		bIsSprinting = false;
		GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : WalkSpeed;
	}
	else
	{
		Server_SetSprinting(false);
	}
}

void AMultiplayerFPSCharacter::StartAim()
{
	if (HasAuthority())
	{
		bIsAiming = true;
		GetCharacterMovement()->MaxWalkSpeed = AimWalkSpeed;
	}
	else
	{
		Server_SetAiming(true);
	}
}

void AMultiplayerFPSCharacter::StopAim()
{
	if (HasAuthority())
	{
		bIsAiming = false;
		GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
	}
	else
	{
		Server_SetAiming(false);
	}
}

void AMultiplayerFPSCharacter::EquipWeapon(TSubclassOf<AWeapon> WeaponClass)
{
	if (!WeaponClass)
	{
		return;
	}

	// Unequip current weapon if any
	UnequipWeapon();

	// Spawn new weapon
	CurrentWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
	if (CurrentWeapon)
	{
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->AttachToComponent(FirstPersonCameraComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		UE_LOG(LogTemp, Warning, TEXT("Weapon equipped!"));
	}
}

void AMultiplayerFPSCharacter::UnequipWeapon()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}
}

void AMultiplayerFPSCharacter::Fire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Fire();
		PlayFireAnimation();
	}
}

void AMultiplayerFPSCharacter::Reload()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Reload();
		PlayReloadAnimation();
	}
}

void AMultiplayerFPSCharacter::StopFiring()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFiring();
	}
}

float AMultiplayerFPSCharacter::TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	CurrentHealth -= Damage;

	if (CurrentHealth <= 0.0f)
	{
		Die();
	}

	return Damage;
}

void AMultiplayerFPSCharacter::ApplyDamage(float Damage)
{
	CurrentHealth -= Damage;

	if (CurrentHealth <= 0.0f)
	{
		Die();
	}
}

void AMultiplayerFPSCharacter::Heal(float Amount)
{
	CurrentHealth = FMath::Min(CurrentHealth + Amount, MaxHealth);
}

void AMultiplayerFPSCharacter::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("Player Died!"));
	// Disable input and ragdoll
	DisableInput(Cast<APlayerController>(GetController()));
	GetCharacterMovement()->SetComponentTickEnabled(false);

	// Schedule respawn
	if (UWorld* World = GetWorld())
	{
		FTimerHandle RespawnTimerHandle;
		World->GetTimerManager().SetTimer(
			RespawnTimerHandle,
			this,
			&AMultiplayerFPSCharacter::Respawn,
			5.0f, // Respawn delay
			false
		);
	}
}

void AMultiplayerFPSCharacter::Respawn()
{
	// Re-enable input and movement
	EnableInput(Cast<APlayerController>(GetController()));
	GetCharacterMovement()->SetComponentTickEnabled(true);

	// Reset health
	CurrentHealth = MaxHealth;

	// Reset position to a spawn point (handled by GameMode)
	if (AMultiplayerFPSGameMode* GameMode = Cast<AMultiplayerFPSGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->RespawnPlayer(Cast<APlayerController>(GetController()));
	}

	UE_LOG(LogTemp, Warning, TEXT("Player respawned!"));
}

void AMultiplayerFPSCharacter::PlayFireAnimation()
{
	if (FireMontage && GetMesh())
	{
		PlayAnimMontage(FireMontage);
	}
}

void AMultiplayerFPSCharacter::PlayReloadAnimation()
{
	if (ReloadMontage && GetMesh())
	{
		PlayAnimMontage(ReloadMontage);
	}
}

void AMultiplayerFPSCharacter::PlayMeleeAnimation()
{
	if (MeleeMontage && GetMesh())
	{
		PlayAnimMontage(MeleeMontage);
	}
}

void AMultiplayerFPSCharacter::UpdateCameraFOV(float DeltaTime)
{
	if (!FirstPersonCameraComponent)
	{
		return;
	}

	float TargetFOV = bIsAiming ? AimFOV : DefaultFOV;
	CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, 10.0f);
	FirstPersonCameraComponent->SetFieldOfView(CurrentFOV);
}

void AMultiplayerFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultiplayerFPSCharacter, CurrentHealth);
	DOREPLIFETIME(AMultiplayerFPSCharacter, bIsAiming);
	DOREPLIFETIME(AMultiplayerFPSCharacter, bIsSprinting);
}

void AMultiplayerFPSCharacter::Server_SetAiming_Implementation(bool bNewAiming)
{
	bIsAiming = bNewAiming;
	GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : (bIsSprinting ? SprintSpeed : WalkSpeed);
}

void AMultiplayerFPSCharacter::Server_SetSprinting_Implementation(bool bNewSprinting)
{
	bIsSprinting = bNewSprinting;
	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? SprintSpeed : (bIsAiming ? AimWalkSpeed : WalkSpeed);
}

void AMultiplayerFPSCharacter::ApplyDamagePublic(float Damage)
{
	ApplyDamage(Damage);
}
