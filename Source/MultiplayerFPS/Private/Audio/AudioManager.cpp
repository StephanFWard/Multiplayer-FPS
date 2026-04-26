#include "Audio/AudioManager.h"
#include "AssetManagement/MultiplayerFPSAssetManager.h"
#include "Weapons/Weapon.h"
#include "Character/MultiplayerFPSCharacter.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

// Initialize static variables
float UAudioManager::MasterVolume = 1.0f;
float UAudioManager::SFXVolume = 1.0f;
float UAudioManager::MusicVolume = 1.0f;
UAudioComponent* UAudioManager::BackgroundMusicComponent = nullptr;

UAudioManager::UAudioManager()
{
}

void UAudioManager::PlaySound2D(const FString& SoundName, float Volume, float Pitch)
{
	USoundBase* Sound = UMultiplayerFPSAssetManager::GetSound(SoundName);
	if (Sound)
	{
		UGameplayStatics::PlaySound2D(GWorld, Sound, Volume * SFXVolume * MasterVolume, Pitch);
	}
}

void UAudioManager::PlaySoundAtLocation(const FString& SoundName, const FVector& Location, float Volume, float Pitch)
{
	USoundBase* Sound = UMultiplayerFPSAssetManager::GetSound(SoundName);
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(GWorld, Sound, Location, FRotator::ZeroRotator,
			Volume * SFXVolume * MasterVolume, Pitch);
	}
}

void UAudioManager::PlayWeaponSound(AWeapon* Weapon, const FString& SoundType)
{
	if (!Weapon)
	{
		return;
	}

	FString SoundName;
	if (SoundType == "Fire")
	{
		SoundName = "Fire";
	}
	else if (SoundType == "Reload")
	{
		SoundName = "Reload";
	}

	if (!SoundName.IsEmpty())
	{
		PlaySoundAtLocation(SoundName, Weapon->GetActorLocation());
	}
}

void UAudioManager::PlayCharacterSound(AMultiplayerFPSCharacter* Character, const FString& SoundType)
{
	if (!Character)
	{
		return;
	}

	FString SoundName;
	if (SoundType == "Hit")
	{
		SoundName = "Hit";
	}
	else if (SoundType == "Death")
	{
		SoundName = "Death";
	}

	if (!SoundName.IsEmpty())
	{
		PlaySoundAtLocation(SoundName, Character->GetActorLocation());
	}
}

void UAudioManager::SetMasterVolume(float Volume)
{
	MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
}

void UAudioManager::SetSFXVolume(float Volume)
{
	SFXVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
}

void UAudioManager::SetMusicVolume(float Volume)
{
	MusicVolume = FMath::Clamp(Volume, 0.0f, 1.0f);

	if (BackgroundMusicComponent)
	{
		BackgroundMusicComponent->SetVolumeMultiplier(MusicVolume * MasterVolume);
	}
}

float UAudioManager::GetMasterVolume()
{
	return MasterVolume;
}

float UAudioManager::GetSFXVolume()
{
	return SFXVolume;
}

float UAudioManager::GetMusicVolume()
{
	return MusicVolume;
}

void UAudioManager::PlayBackgroundMusic(const FString& MusicName)
{
	USoundBase* Music = UMultiplayerFPSAssetManager::GetSound(MusicName);
	if (Music)
	{
		// Stop existing music
		StopBackgroundMusic();

		// Create new audio component
		BackgroundMusicComponent = UGameplayStatics::CreateSound2D(GWorld, Music, 1.0f, 1.0f, 0.0f);
		if (BackgroundMusicComponent)
		{
			BackgroundMusicComponent->SetVolumeMultiplier(MusicVolume * MasterVolume);
			BackgroundMusicComponent->Play();
		}
	}
}

void UAudioManager::StopBackgroundMusic()
{
	if (BackgroundMusicComponent)
	{
		BackgroundMusicComponent->Stop();
		BackgroundMusicComponent = nullptr;
	}
}