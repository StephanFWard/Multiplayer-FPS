#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TestManager.generated.h"

/**
 * Manages automated testing for the game systems
 */
UCLASS()
class MULTIPLAYERFPS_API UTestManager : public UObject
{
	GENERATED_BODY()

public:
	UTestManager();

	// Test execution
	void RunAllTests();
	void RunUnitTests();
	void RunIntegrationTests();
	void RunPerformanceTests();

	// Test results
	int32 GetTotalTests() const { return TotalTests; }
	int32 GetPassedTests() const { return PassedTests; }
	int32 GetFailedTests() const { return FailedTests; }
	float GetTestCoverage() const;

	// Individual test functions
	bool TestCharacterMovement();
	bool TestWeaponFiring();
	bool TestDamageSystem();
	bool TestAssetLoading();
	bool TestMultiplayerSession();
	bool TestAIBehavior();
	bool TestPickupSystem();
	bool TestLevelGeneration();

	// Performance tests
	float TestFrameRate();
	float TestMemoryUsage();
	float TestNetworkLatency();

	// Test reporting
	FString GenerateTestReport() const;
	void LogTestResult(const FString& TestName, bool bPassed, const FString& Details = "");

protected:
	// Test counters
	int32 TotalTests;
	int32 PassedTests;
	int32 FailedTests;

	// Test results
	TArray<FString> TestResults;

private:
	// Helper functions
	void ResetTestCounters();
	void RecordTestResult(const FString& TestName, bool bPassed, const FString& Details = "");
	bool AssertTrue(bool Condition, const FString& Message);
	bool AssertFalse(bool Condition, const FString& Message);
	bool AssertEqual(int32 A, int32 B, const FString& Message);
	bool AssertEqual(float A, float B, const FString& Message, float Tolerance = 0.01f);
};