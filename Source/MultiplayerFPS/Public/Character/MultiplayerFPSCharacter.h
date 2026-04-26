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
UCLASS()
class MULTIPLAYERFPS_API AMultiplayerFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMultiplayerFPSCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AimWalkSpeed = 300.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsAiming = false;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsSprinting = false;

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

	// ========== Health & Damage ==========
	
	UFUNCTION(BlueprintCallable, Category = "Health")
	void TakeDamage(float Damage);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Die();

	// ========== Weapon System ==========
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EquipWeapon(TSubclassOf<AWeapon> WeaponClass);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void UnequipWeapon();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Fire();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StopFiring();

	// ========== Movement Callbacks ==========
	
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
