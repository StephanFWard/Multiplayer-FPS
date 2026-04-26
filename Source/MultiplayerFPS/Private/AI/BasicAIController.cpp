#include "AI/BasicAIController.h"
#include "Character/MultiplayerFPSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ABasicAIController::ABasicAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABasicAIController::BeginPlay()
{
	Super::BeginPlay();

	GeneratePatrolPoints();
}

void ABasicAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (CurrentState)
	{
	case EAIState::Patrolling:
		Patrol();
		break;
	case EAIState::Chasing:
		if (CurrentTarget)
		{
			ChasePlayer(CurrentTarget);
		}
		else
		{
			CurrentState = EAIState::Patrolling;
		}
		break;
	case EAIState::Attacking:
		if (CurrentTarget)
		{
			AttackPlayer(CurrentTarget);
		}
		else
		{
			CurrentState = EAIState::Patrolling;
		}
		break;
	case EAIState::Searching:
		SearchForPlayer();
		break;
	}

	// Check for player detection
	AMultiplayerFPSCharacter* NearestPlayer = FindNearestPlayer();
	if (NearestPlayer)
	{
		float Distance = FVector::Dist(GetPawn()->GetActorLocation(), NearestPlayer->GetActorLocation());

		if (Distance <= AttackRange && CanSeePlayer(NearestPlayer))
		{
			CurrentState = EAIState::Attacking;
			CurrentTarget = NearestPlayer;
		}
		else if (Distance <= DetectionRange && CanSeePlayer(NearestPlayer))
		{
			CurrentState = EAIState::Chasing;
			CurrentTarget = NearestPlayer;
		}
	}
	else
	{
		// Lost target
		if (CurrentState != EAIState::Patrolling)
		{
			CurrentState = EAIState::Searching;
		}
	}
}

void ABasicAIController::Patrol()
{
	if (PatrolPoints.Num() == 0)
	{
		return;
	}

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	// Move to next patrol point
	FVector TargetLocation = GetNextPatrolPoint();
	MoveToLocation(TargetLocation, 50.0f, false);

	// Update movement speed
	if (UCharacterMovementComponent* Movement = Cast<UCharacterMovementComponent>(ControlledPawn->GetMovementComponent()))
	{
		Movement->MaxWalkSpeed = PatrolSpeed;
	}

	// Check if reached patrol point
	if (FVector::Dist(ControlledPawn->GetActorLocation(), TargetLocation) < 100.0f)
	{
		CurrentPatrolIndex = (CurrentPatrolIndex + 1) % PatrolPoints.Num();
	}
}

void ABasicAIController::ChasePlayer(AMultiplayerFPSCharacter* TargetPlayer)
{
	if (!TargetPlayer)
	{
		return;
	}

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	// Move towards player
	MoveToActor(TargetPlayer, AttackRange - 50.0f, true, true, false, 0, true);

	// Update movement speed
	if (UCharacterMovementComponent* Movement = Cast<UCharacterMovementComponent>(ControlledPawn->GetMovementComponent()))
	{
		Movement->MaxWalkSpeed = ChaseSpeed;
	}
}

void ABasicAIController::AttackPlayer(AMultiplayerFPSCharacter* TargetPlayer)
{
	if (!TargetPlayer)
	{
		return;
	}

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	// Stop moving and face player
	StopMovement();
	ControlledPawn->SetActorRotation(
		(TargetPlayer->GetActorLocation() - ControlledPawn->GetActorLocation()).Rotation()
	);

	// Attack if cooldown is over
	if (GetWorld()->GetTimeSeconds() - LastAttackTime >= AttackCooldown)
	{
		// Deal damage to player
		TargetPlayer->TakeDamage(25.0f); // AI damage
		LastAttackTime = GetWorld()->GetTimeSeconds();

		UE_LOG(LogTemp, Warning, TEXT("AI attacked player for 25 damage"));
	}
}

void ABasicAIController::SearchForPlayer()
{
	// Simple search behavior - move to last known position or random location
	// For now, just transition back to patrolling
	CurrentState = EAIState::Patrolling;
	CurrentTarget = nullptr;
}

bool ABasicAIController::CanSeePlayer(AMultiplayerFPSCharacter* Player) const
{
	if (!Player)
	{
		return false;
	}

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return false;
	}

	// Simple line of sight check
	FVector Start = ControlledPawn->GetActorLocation();
	FVector End = Player->GetActorLocation();

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(ControlledPawn);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams))
	{
		return HitResult.GetActor() == Player;
	}

	return false;
}

AMultiplayerFPSCharacter* ABasicAIController::FindNearestPlayer() const
{
	TArray<AActor*> FoundPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMultiplayerFPSCharacter::StaticClass(), FoundPlayers);

	AMultiplayerFPSCharacter* NearestPlayer = nullptr;
	float NearestDistance = DetectionRange;

	for (AActor* Actor : FoundPlayers)
	{
		AMultiplayerFPSCharacter* Player = Cast<AMultiplayerFPSCharacter>(Actor);
		if (Player && Player != GetPawn()) // Don't target self
		{
			float Distance = FVector::Dist(GetPawn()->GetActorLocation(), Player->GetActorLocation());
			if (Distance < NearestDistance)
			{
				NearestDistance = Distance;
				NearestPlayer = Player;
			}
		}
	}

	return NearestPlayer;
}

void ABasicAIController::GeneratePatrolPoints()
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	FVector Center = ControlledPawn->GetActorLocation();

	// Generate 4 patrol points in a square around the spawn location
	PatrolPoints.Add(Center + FVector(500.0f, 0.0f, 0.0f));
	PatrolPoints.Add(Center + FVector(0.0f, 500.0f, 0.0f));
	PatrolPoints.Add(Center + FVector(-500.0f, 0.0f, 0.0f));
	PatrolPoints.Add(Center + FVector(0.0f, -500.0f, 0.0f));
}

FVector ABasicAIController::GetNextPatrolPoint()
{
	if (PatrolPoints.Num() == 0)
	{
		return GetPawn() ? GetPawn()->GetActorLocation() : FVector::ZeroVector;
	}

	return PatrolPoints[CurrentPatrolIndex];
}