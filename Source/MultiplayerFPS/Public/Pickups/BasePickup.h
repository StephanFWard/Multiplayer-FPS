#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasePickup.generated.h"

class USphereComponent;
class UStaticMeshComponent;

/**
 * Base class for all pickup items (ammo, health, etc.)
 */
UCLASS()
class MULTIPLAYERFPS_API ABasePickup : public AActor
{
	GENERATED_BODY()

public:
	ABasePickup();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Pickup interaction
	virtual void OnPickup(class AMultiplayerFPSCharacter* Player);
	virtual bool CanPickup(class AMultiplayerFPSCharacter* Player) const;

	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	// Pickup properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	FString PickupName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	float RespawnTime = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	bool bAutoRespawn = true;

	// Replication
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Pickup")
	bool bIsActive = true;

	float RespawnTimer = 0.0f;

	// Visual effects
	void PlayPickupEffect();
	void PlayRespawnEffect();

	// Overlap events
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	// Respawn logic
	void DeactivatePickup();
	void RespawnPickup();
};