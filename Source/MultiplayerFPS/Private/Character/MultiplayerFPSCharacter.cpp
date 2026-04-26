#include "Character/MultiplayerFPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Weapons/Weapon.h"
#include "InputMappingContext.h"

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
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::StartedTriggered, this, &ACharacter::Jump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::CompletedTriggered, this, &ACharacter::StopJumping);
		}

		// Sprinting
		if (SprintAction)
		{
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::StartedTriggered, this, &AMultiplayerFPSCharacter::StartSprint);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::CompletedTriggered, this, &AMultiplayerFPSCharacter::StopSprint);
		}

		// Aiming
		if (AimAction)
		{
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::StartedTriggered, this, &AMultiplayerFPSCharacter::StartAim);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::CompletedTriggered, this, &AMultiplayerFPSCharacter::StopAim);
		}

		// Firing
		if (FireAction)
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::StartedTriggered, this, &AMultiplayerFPSCharacter::Fire);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::CompletedTriggered, this, &AMultiplayerFPSCharacter::StopFiring);
		}
	}
}

void AMultiplayerFPSCharacter::Move(const FInputActionValue& Value)
{
	if (const FVector2D MovementVector = Value.Get<FVector2D>())
	{
		// Move forward/backward
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);

		// Move left/right
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AMultiplayerFPSCharacter::Look(const FInputActionValue& Value)
{
	if (const FVector2D LookAxisVector = Value.Get<FVector2D>())
	{
		// Look up/down
		AddControllerPitchInput(LookAxisVector.Y);

		// Look left/right
		AddControllerYawInput(LookAxisVector.X);
	}
}

void AMultiplayerFPSCharacter::StartSprint()
{
	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AMultiplayerFPSCharacter::StopSprint()
{
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : WalkSpeed;
}

void AMultiplayerFPSCharacter::StartAim()
{
	bIsAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = AimWalkSpeed;
}

void AMultiplayerFPSCharacter::StopAim()
{
	bIsAiming = false;
	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
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
	}
}

void AMultiplayerFPSCharacter::StopFiring()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFiring();
	}
}

void AMultiplayerFPSCharacter::TakeDamage(float Damage)
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
}
