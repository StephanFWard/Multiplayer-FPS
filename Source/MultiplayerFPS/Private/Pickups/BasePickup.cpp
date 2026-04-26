#include "Pickups/BasePickup.h"
#include "Character/MultiplayerFPSCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

ABasePickup::ABasePickup()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create collision component
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(50.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("OverlapAll"));
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABasePickup::OnOverlapBegin);
	RootComponent = CollisionComponent;

	// Create mesh component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Set replication
	bReplicates = true;
	bIsActive = true;
}

void ABasePickup::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		RespawnTimer = 0.0f;
	}
}

void ABasePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && !bIsActive && bAutoRespawn)
	{
		RespawnTimer += DeltaTime;
		if (RespawnTimer >= RespawnTime)
		{
			RespawnPickup();
		}
	}
}

void ABasePickup::OnPickup(AMultiplayerFPSCharacter* Player)
{
	// Base implementation - override in derived classes
	UE_LOG(LogTemp, Log, TEXT("Base pickup collected by %s"), *Player->GetName());

	DeactivatePickup();
}

bool ABasePickup::CanPickup(AMultiplayerFPSCharacter* Player) const
{
	return bIsActive && Player != nullptr;
}

void ABasePickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABasePickup, bIsActive);
}

void ABasePickup::PlayPickupEffect()
{
	// Play pickup sound and particle effects
	// Implementation depends on asset manager
}

void ABasePickup::PlayRespawnEffect()
{
	// Play respawn sound and particle effects
	// Implementation depends on asset manager
}

void ABasePickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (HasAuthority() && bIsActive)
	{
		AMultiplayerFPSCharacter* Player = Cast<AMultiplayerFPSCharacter>(OtherActor);
		if (Player && CanPickup(Player))
		{
			OnPickup(Player);
		}
	}
}

void ABasePickup::DeactivatePickup()
{
	bIsActive = false;
	MeshComponent->SetVisibility(false);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PlayPickupEffect();
}

void ABasePickup::RespawnPickup()
{
	bIsActive = true;
	MeshComponent->SetVisibility(true);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	RespawnTimer = 0.0f;

	PlayRespawnEffect();
}