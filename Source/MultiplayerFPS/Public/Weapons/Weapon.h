#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class USkeletalMeshComponent;

/**
 * Base weapon class for all weapons in the game
 * Handles weapon firing, ammo management, and weapon state
 */
UCLASS()
class MULTIPLAYERFPS_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// ========== Components ==========
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	USkeletalMeshComponent* MeshComponent;

	// ========== Weapon Properties ==========
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float Damage = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float FireRate = 0.1f; // Time between shots in seconds

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float Range = 10000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", Replicated)
	int32 MagazineCapacity = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", Replicated)
	int32 TotalAmmo = 300;

	UPROPERTY(BlueprintReadOnly, Category = "Ammo", Replicated)
	int32 CurrentAmmo;

	// ========== Animation & Effects ==========
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* FireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	USoundBase* ReloadSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TSubclassOf<AActor> BulletImpactEffect;

	// ========== Reload System ==========
	
	UPROPERTY(BlueprintReadOnly, Category = "Reload", Replicated)
	bool bIsReloading = false;

	float ReloadStartTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reload")
	float ReloadTime = 2.0f;

	// ========== Firing System ==========
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Fire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void StopFiring();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Reload();

	// ========== Effects ==========
	
	UFUNCTION(BlueprintCallable, Category = "Effects")
	void PlayMuzzleFlash();

	UFUNCTION(BlueprintCallable, Category = "Effects")
	void PlayFireSound();

	UFUNCTION(BlueprintCallable, Category = "Effects")
	void SpawnBulletImpact(const FVector& Location, const FVector& Normal);

	// ========== Reload System ==========
	
	UFUNCTION(BlueprintCallable, Category = "Reload")
	bool CanReload() const;

	UFUNCTION(BlueprintCallable, Category = "Reload")
	void StartReload();

	UFUNCTION(BlueprintCallable, Category = "Reload")
	void FinishReload();
	void PerformRaycast();

	// ========== Ammo Management ==========
	
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	void AddAmmo(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Ammo")
	bool HasAmmo() const;

public:
	// Getter functions
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	float GetDamage() const { return Damage; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	float GetFireRate() const { return FireRate; }

	UFUNCTION(BlueprintCallable, Category = "Ammo")
	int32 GetCurrentAmmo() const { return CurrentAmmo; }

	UFUNCTION(BlueprintCallable, Category = "Ammo")
	int32 GetMagazineCapacity() const { return MagazineCapacity; }

	UFUNCTION(BlueprintCallable, Category = "Ammo")
	int32 GetTotalAmmo() const { return TotalAmmo; }
};
