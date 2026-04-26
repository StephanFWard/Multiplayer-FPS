#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "MultiplayerFPSAssetManager.generated.h"

/**
 * Custom asset manager for loading and managing game assets
 */
UCLASS()
class MULTIPLAYERFPS_API UMultiplayerFPSAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	UMultiplayerFPSAssetManager();

	virtual void StartInitialLoading() override;

	// Asset loading functions
	UFUNCTION(BlueprintCallable, Category = "AssetManager")
	static USkeletalMesh* GetCharacterMesh(const FString& CharacterType);

	UFUNCTION(BlueprintCallable, Category = "AssetManager")
	static USkeletalMesh* GetWeaponMesh(const FString& WeaponType);

	UFUNCTION(BlueprintCallable, Category = "AssetManager")
	static UAnimMontage* GetAnimationMontage(const FString& AnimationName);

	UFUNCTION(BlueprintCallable, Category = "AssetManager")
	static UMaterialInterface* GetMaterial(const FString& MaterialName);

	UFUNCTION(BlueprintCallable, Category = "AssetManager")
	static USoundBase* GetSound(const FString& SoundName);

	UFUNCTION(BlueprintCallable, Category = "AssetManager")
	static UParticleSystem* GetParticleSystem(const FString& ParticleName);

	UFUNCTION(BlueprintCallable, Category = "AssetManager")
	static UStaticMesh* GetStaticMesh(const FString& MeshName);

private:
	// Asset caches
	static TMap<FString, TSoftObjectPtr<USkeletalMesh>> CharacterMeshes;
	static TMap<FString, TSoftObjectPtr<USkeletalMesh>> WeaponMeshes;
	static TMap<FString, TSoftObjectPtr<UAnimMontage>> AnimationMontages;
	static TMap<FString, TSoftObjectPtr<UMaterialInterface>> Materials;
	static TMap<FString, TSoftObjectPtr<USoundBase>> Sounds;
	static TMap<FString, TSoftObjectPtr<UParticleSystem>> ParticleSystems;
	static TMap<FString, TSoftObjectPtr<UStaticMesh>> StaticMeshes;

	// Initialize asset maps
	void InitializeAssetMaps();
};