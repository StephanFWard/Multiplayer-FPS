#include "AssetManagement/MultiplayerFPSAssetManager.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimMontage.h"
#include "Materials/MaterialInterface.h"
#include "Sound/SoundBase.h"
#include "Particles/ParticleSystem.h"
#include "Engine/StaticMesh.h"

// Initialize static maps
TMap<FString, TSoftObjectPtr<USkeletalMesh>> UMultiplayerFPSAssetManager::CharacterMeshes;
TMap<FString, TSoftObjectPtr<USkeletalMesh>> UMultiplayerFPSAssetManager::WeaponMeshes;
TMap<FString, TSoftObjectPtr<UAnimMontage>> UMultiplayerFPSAssetManager::AnimationMontages;
TMap<FString, TSoftObjectPtr<UMaterialInterface>> UMultiplayerFPSAssetManager::Materials;
TMap<FString, TSoftObjectPtr<USoundBase>> UMultiplayerFPSAssetManager::Sounds;
TMap<FString, TSoftObjectPtr<UParticleSystem>> UMultiplayerFPSAssetManager::ParticleSystems;
TMap<FString, TSoftObjectPtr<UStaticMesh>> UMultiplayerFPSAssetManager::StaticMeshes;

UMultiplayerFPSAssetManager::UMultiplayerFPSAssetManager()
{
}

void UMultiplayerFPSAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	InitializeAssetMaps();
}

void UMultiplayerFPSAssetManager::InitializeAssetMaps()
{
	// Character meshes
	CharacterMeshes.Add("Policeman", TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath("/Game/Models/Characters/Policeman.Policeman")));
	CharacterMeshes.Add("RobotX", TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath("/Game/Models/Characters/Robot_X.Robot_X")));
	CharacterMeshes.Add("RobotY", TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath("/Game/Models/Characters/Robot_Y.Robot_Y")));

	// Weapon meshes
	WeaponMeshes.Add("Gun", TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath("/Game/Models/Weapons/Gun.Gun")));

	// Animation montages
	AnimationMontages.Add("Fire", TSoftObjectPtr<UAnimMontage>(FSoftObjectPath("/Game/Animations/Montages/Fire_Montage.Fire_Montage")));
	AnimationMontages.Add("Reload", TSoftObjectPtr<UAnimMontage>(FSoftObjectPath("/Game/Animations/Montages/Reload_Montage.Reload_Montage")));
	AnimationMontages.Add("Melee", TSoftObjectPtr<UAnimMontage>(FSoftObjectPath("/Game/Animations/Montages/Melee_Montage.Melee_Montage")));

	// Materials
	Materials.Add("DefaultCharacter", TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath("/Game/Materials/Character_Default.Character_Default")));
	Materials.Add("BulletImpact", TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath("/Game/Materials/Bullet_Impact.Bullet_Impact")));

	// Sounds
	Sounds.Add("Fire", TSoftObjectPtr<USoundBase>(FSoftObjectPath("/Game/Audio/Sounds/Gun_Fire.Gun_Fire")));
	Sounds.Add("Reload", TSoftObjectPtr<USoundBase>(FSoftObjectPath("/Game/Audio/Sounds/Gun_Reload.Gun_Reload")));
	Sounds.Add("Hit", TSoftObjectPtr<USoundBase>(FSoftObjectPath("/Game/Audio/Sounds/Hit_Sound.Hit_Sound")));

	// Particle systems
	ParticleSystems.Add("MuzzleFlash", TSoftObjectPtr<UParticleSystem>(FSoftObjectPath("/Game/Particles/Muzzle_Flash.Muzzle_Flash")));
	ParticleSystems.Add("Blood", TSoftObjectPtr<UParticleSystem>(FSoftObjectPath("/Game/Particles/Blood_Spray.Blood_Spray")));

	// Static meshes
	StaticMeshes.Add("BulletImpact", TSoftObjectPtr<UStaticMesh>(FSoftObjectPath("/Game/Models/Environment/BulletImpacts.BulletImpacts")));
	StaticMeshes.Add("Environment", TSoftObjectPtr<UStaticMesh>(FSoftObjectPath("/Game/Models/Environment/Enviroment.Enviroment")));
}

USkeletalMesh* UMultiplayerFPSAssetManager::GetCharacterMesh(const FString& CharacterType)
{
	TSoftObjectPtr<USkeletalMesh>* MeshPtr = CharacterMeshes.Find(CharacterType);
	if (MeshPtr && MeshPtr->IsValid())
	{
		return MeshPtr->LoadSynchronous();
	}
	return nullptr;
}

USkeletalMesh* UMultiplayerFPSAssetManager::GetWeaponMesh(const FString& WeaponType)
{
	TSoftObjectPtr<USkeletalMesh>* MeshPtr = WeaponMeshes.Find(WeaponType);
	if (MeshPtr && MeshPtr->IsValid())
	{
		return MeshPtr->LoadSynchronous();
	}
	return nullptr;
}

UAnimMontage* UMultiplayerFPSAssetManager::GetAnimationMontage(const FString& AnimationName)
{
	TSoftObjectPtr<UAnimMontage>* MontagePtr = AnimationMontages.Find(AnimationName);
	if (MontagePtr && MontagePtr->IsValid())
	{
		return MontagePtr->LoadSynchronous();
	}
	return nullptr;
}

UMaterialInterface* UMultiplayerFPSAssetManager::GetMaterial(const FString& MaterialName)
{
	TSoftObjectPtr<UMaterialInterface>* MaterialPtr = Materials.Find(MaterialName);
	if (MaterialPtr && MaterialPtr->IsValid())
	{
		return MaterialPtr->LoadSynchronous();
	}
	return nullptr;
}

USoundBase* UMultiplayerFPSAssetManager::GetSound(const FString& SoundName)
{
	TSoftObjectPtr<USoundBase>* SoundPtr = Sounds.Find(SoundName);
	if (SoundPtr && SoundPtr->IsValid())
	{
		return SoundPtr->LoadSynchronous();
	}
	return nullptr;
}

UParticleSystem* UMultiplayerFPSAssetManager::GetParticleSystem(const FString& ParticleName)
{
	TSoftObjectPtr<UParticleSystem>* ParticlePtr = ParticleSystems.Find(ParticleName);
	if (ParticlePtr && ParticlePtr->IsValid())
	{
		return ParticlePtr->LoadSynchronous();
	}
	return nullptr;
}

UStaticMesh* UMultiplayerFPSAssetManager::GetStaticMesh(const FString& MeshName)
{
	TSoftObjectPtr<UStaticMesh>* MeshPtr = StaticMeshes.Find(MeshName);
	if (MeshPtr && MeshPtr->IsValid())
	{
		return MeshPtr->LoadSynchronous();
	}
	return nullptr;
}