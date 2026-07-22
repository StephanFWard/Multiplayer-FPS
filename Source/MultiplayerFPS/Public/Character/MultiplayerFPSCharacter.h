#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MultiplayerFPSCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UEnhancedInputComponent;
class UEnhancedInputSubsystem;
struct FInputActionValue;
class AWeapon;

/**
 * Main player character class for Multiplayer FPS
 * Handles movement, combat, and first-person perspective
 */
UCLASS(config=Game)
class MULTIPLAYERFPS_API AMultiplayerFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMultiplayerFPSCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ========== Replication Functions ==========
	
	UFUNCTION(Server, Reliable)
	void Server_SetAiming(bool bNewAiming);

	UFUNCTION(Server, Reliable)
	void Server_SetSprinting(bool bNewSprinting);

protected:
	// ========== Components ==========
	
	// First-person camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FirstPersonCameraComponent;

	// Current equipped weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AWeapon> DefaultWeaponClass;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	AWeapon* CurrentWeapon;

	// ========== Character Stats ==========
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", Replicated)
	float MaxHealth = 100.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Health", Replicated)
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", Replicated)
	float WalkSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", Replicated)
	float SprintSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", Replicated)
	float AimWalkSpeed = 300.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", Replicated)
	bool bIsAiming = false;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", Replicated)
	bool bIsSprinting = false;

	// ========== Animation System ==========
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* FireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* MeleeMontage;

	// ========== Camera Effects ==========
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float AimFOV = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float DefaultFOV = 90.0f;

	float CurrentFOV;

	// ========== Input System ==========
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* ReloadAction;
	
	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// Public wrapper for AI damage
	UFUNCTION(BlueprintCallable, Category = "Health")
	void ApplyDamage(float Damage);

public:
	// Public wrapper for AI damage (public access for AI)
	UFUNCTION(BlueprintCallable, Category = "Health")
	void ApplyDamagePublic(float Damage);

	// Setup input component
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Die();

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Respawn();

	// ========== Weapon System ==========
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EquipWeapon(TSubclassOf<AWeapon> WeaponClass);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void UnequipWeapon();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Fire();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Reload();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StopFiring();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayFireAnimation();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayReloadAnimation();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayMeleeAnimation();

	// ========== Camera Effects ==========
	
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void UpdateCameraFOV(float DeltaTime);
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartSprint();
	void StopSprint();
	void StartAim();
	void StopAim();

public:
	// Getter functions
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealthPercentage() const { return CurrentHealth / MaxHealth; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	AWeapon* GetCurrentWeapon() const { return CurrentWeapon; }

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsAiming() const { return bIsAiming; }

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsSprinting() const { return bIsSprinting; }
};
