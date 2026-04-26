#include "Environment/DoorActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Character/MultiplayerFPSCharacter.h"

ADoorActor::ADoorActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create door mesh
	DoorMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DoorMesh"));
	RootComponent = DoorMesh;

	// Create trigger box for detection
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetCollisionEnabled(ECC_WorldDynamic);
	TriggerBox->SetGenerateOverlapEvents(true);

	// Bind overlap events
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ADoorActor::OnTriggerBoxOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ADoorActor::OnTriggerBoxOverlapEnd);
}

void ADoorActor::BeginPlay()
{
	Super::BeginPlay();
}

void ADoorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update door rotation
	UpdateDoorRotation(DeltaTime);
}

void ADoorActor::OnTriggerBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if overlapping actor is a player
	if (OtherActor && OtherActor->IsA<AMultiplayerFPSCharacter>())
	{
		OpenDoor();
	}
}

void ADoorActor::OnTriggerBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Check if leaving actor is a player
	if (OtherActor && OtherActor->IsA<AMultiplayerFPSCharacter>())
	{
		CloseDoor();
	}
}

void ADoorActor::OpenDoor()
{
	bIsOpen = true;
	TargetDoorRotation = DoorOpenAngle;
	UE_LOG(LogTemp, Warning, TEXT("Door opening!"));
}

void ADoorActor::CloseDoor()
{
	bIsOpen = false;
	TargetDoorRotation = DoorCloseAngle;
	UE_LOG(LogTemp, Warning, TEXT("Door closing!"));
}

void ADoorActor::UpdateDoorRotation(float DeltaTime)
{
	// Smoothly rotate door towards target
	if (!FMath::IsNearlyEqual(CurrentDoorRotation, TargetDoorRotation, 0.5f))
	{
		float RotationAmount = DoorSpeed * DeltaTime;
		
		if (CurrentDoorRotation < TargetDoorRotation)
		{
			CurrentDoorRotation = FMath::Min(CurrentDoorRotation + RotationAmount, TargetDoorRotation);
		}
		else
		{
			CurrentDoorRotation = FMath::Max(CurrentDoorRotation - RotationAmount, TargetDoorRotation);
		}

		// Apply rotation to door mesh
		FRotator NewRotation = DoorMesh->GetComponentRotation();
		NewRotation.Yaw = CurrentDoorRotation;
		DoorMesh->SetComponentRotation(NewRotation);
	}
}
