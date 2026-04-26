#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorActor.generated.h"

class UBoxComponent;
class USkeletalMeshComponent;

/**
 * Interactive door that opens when a player is near
 * Automatically closes when player moves away
 */
UCLASS()
class MULTIPLAYERFPS_API ADoorActor : public AActor
{
	GENERATED_BODY()

public:
	ADoorActor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	// ========== Components ==========
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	USkeletalMeshComponent* DoorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	UBoxComponent* TriggerBox;

	// ========== Door State ==========
	
	UPROPERTY(BlueprintReadOnly, Category = "Door")
	bool bIsOpen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float DoorOpenAngle = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float DoorCloseAngle = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float DoorSpeed = 45.0f; // Degrees per second

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float DoorDetectionRange = 200.0f;

	// Current rotation of the door
	float CurrentDoorRotation = 0.0f;
	float TargetDoorRotation = 0.0f;

	// Overlap events
	UFUNCTION()
	void OnTriggerBoxOverlapBegin(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, 
		class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerBoxOverlapEnd(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Door control
	UFUNCTION(BlueprintCallable, Category = "Door")
	void OpenDoor();

	UFUNCTION(BlueprintCallable, Category = "Door")
	void CloseDoor();

	void UpdateDoorRotation(float DeltaTime);

public:
	UFUNCTION(BlueprintCallable, Category = "Door")
	bool IsOpen() const { return bIsOpen; }
};
