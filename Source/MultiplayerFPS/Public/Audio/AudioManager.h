#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AudioManager.generated.h"

/**
 * Audio manager for handling sound playback and audio settings
 */
UCLASS()
class MULTIPLAYERFPS_API UAudioManager : public UObject
{
	GENERATED_BODY()

public:
	UAudioManager();

	// Sound playback functions
	UFUNCTION(BlueprintCallable, Category = "Audio")
	static void PlaySound2D(const FString& SoundName, float Volume = 1.0f, float Pitch = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	static void PlaySoundAtLocation(const FString& SoundName, const FVector& Location, float Volume = 1.0f, float Pitch = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	static void PlayWeaponSound(class AWeapon* Weapon, const FString& SoundType);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	static void PlayCharacterSound(class AMultiplayerFPSCharacter* Character, const FString& SoundType);

	// Audio settings
	UFUNCTION(BlueprintCallable, Category = "Audio")
	static void SetMasterVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	static void SetSFXVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	static void SetMusicVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	static float GetMasterVolume();

	UFUNCTION(BlueprintCallable, Category = "Audio")
	static float GetSFXVolume();

	UFUNCTION(BlueprintCallable, Category = "Audio")
	static float GetMusicVolume();

	// Background music
	UFUNCTION(BlueprintCallable, Category = "Audio")
	static void PlayBackgroundMusic(const FString& MusicName);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	static void StopBackgroundMusic();

private:
	static float MasterVolume;
	static float SFXVolume;
	static float MusicVolume;

	static class UAudioComponent* BackgroundMusicComponent;
};