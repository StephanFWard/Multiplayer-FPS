#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BasicAIController.generated.h"

/**
 * Basic AI controller for enemy characters
 */
UCLASS()
class MULTIPLAYERFPS_API ABasicAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABasicAIController();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// AI behavior functions
	void Patrol();
	void ChasePlayer(class AMultiplayerFPSCharacter* TargetPlayer);
	void AttackPlayer(class AMultiplayerFPSCharacter* TargetPlayer);
	void SearchForPlayer();

	// Detection
	bool CanSeePlayer(class AMultiplayerFPSCharacter* Player) const;
	class AMultiplayerFPSCharacter* FindNearestPlayer() const;

protected:
	// AI properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DetectionRange = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackRange = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float PatrolSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float ChaseSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackCooldown = 2.0f;

	// AI state
	enum class EAIState { Patrolling, Chasing, Attacking, Searching };
	EAIState CurrentState = EAIState::Patrolling;

	class AMultiplayerFPSCharacter* CurrentTarget = nullptr;
	float LastAttackTime = 0.0f;

	// Patrol points
	TArray<FVector> PatrolPoints;
	int32 CurrentPatrolIndex = 0;

public:
	void GeneratePatrolPoints();
	FVector GetNextPatrolPoint();
	
	const TArray<FVector>& GetPatrolPoints() const { return PatrolPoints; }
	
	// Public accessors for testing
	void GeneratePatrolPointsPublic() { GeneratePatrolPoints(); }
	const TArray<FVector>& GetPatrolPointsPublic() const { return PatrolPoints; }
};
