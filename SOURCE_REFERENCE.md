# Multiplayer FPS - C++ Source File Reference

Complete reference of all C++ classes and their functionality in the Unreal Engine 5 conversion.

## Core Game Classes

### 1. AMultiplayerFPSCharacter
**Location:** `Source/MultiplayerFPS/Public/Character/MultiplayerFPSCharacter.h/cpp`  
**Inherits From:** `ACharacter`  
**Purpose:** Main player character class handling all player-related functionality

#### Key Features:
- First-person camera component
- Enhanced input system integration (Move, Look, Sprint, Aim, Fire)
- Movement states (walking, sprinting, aiming)
- Health and damage system
- Weapon equipping and firing
- Animation and state management

#### Key Methods:
- `Move()` - Handle movement input
- `Look()` - Handle camera rotation
- `Fire()` / `StopFiring()` - Weapon firing control
- `StartAim()` / `StopAim()` - Aiming state
- `StartSprint()` / `StopSprint()` - Sprint state
- `TakeDamage()` - Apply damage to character
- `EquipWeapon()` / `UnequipWeapon()` - Weapon management
- `Die()` - Character death

#### Key Properties:
- `CurrentWeapon` - Active equipped weapon
- `CurrentHealth` / `MaxHealth` - Health tracking
- `WalkSpeed`, `SprintSpeed`, `AimWalkSpeed` - Movement speeds
- `bIsAiming`, `bIsSprinting` - State flags

---

### 2. AWeapon
**Location:** `Source/MultiplayerFPS/Public/Weapons/Weapon.h/cpp`  
**Inherits From:** `AActor`  
**Purpose:** Base weapon class for all weapons (guns, melee, etc.)

#### Key Features:
- Skeletal mesh component for weapon model
- Raycast-based firing system (hitscan)
- Ammo management (current magazine, total ammo)
- Fire rate limiting
- Damage application to hit actors

#### Key Methods:
- `Fire()` - Execute weapon fire
- `StopFiring()` - Stop active firing
- `PerformRaycast()` - Execute line trace for hitscan
- `Reload()` - Reload weapon
- `AddAmmo()` - Add ammo to inventory
- `HasAmmo()` - Check if weapon has ammo

#### Key Properties:
- `CurrentAmmo` / `MagazineCapacity` - Magazine management
- `TotalAmmo` - Total ammunition available
- `Damage` - Damage per shot
- `FireRate` - Cooldown between shots
- `Range` - Raycast detection range

---

### 3. AMultiplayerFPSGameMode
**Location:** `Source/MultiplayerFPS/Public/Game/MultiplayerFPSGameMode.h/cpp`  
**Inherits From:** `AGameModeBase`  
**Purpose:** Controls game rules, spawning, and round management

#### Key Features:
- Player spawn management
- Game round timing
- Player count tracking
- Game state management (start, end, respawn)

#### Key Methods:
- `StartGame()` - Initialize game round
- `EndGame()` - Finish game round
- `RespawnPlayer()` - Handle player respawning
- `GetCurrentPlayerCount()` - Get active players
- `GetRoundTime()` - Get remaining round time

#### Key Properties:
- `MaxPlayers` - Maximum players per session (default: 8)
- `RespawnDelay` - Time before player respawns (default: 5s)
- `RoundTime` - Total round duration (default: 600s / 10min)
- `CurrentPlayerCount` - Active player count

---

### 4. AMultiplayerFPSPlayerController
**Location:** `Source/MultiplayerFPS/Public/Game/MultiplayerFPSPlayerController.h/cpp`  
**Inherits From:** `APlayerController`  
**Purpose:** Manages individual player input, statistics, and HUD

#### Key Features:
- Player name tracking
- Kill/death statistics
- K/D ratio calculation
- HUD management
- Input coordination

#### Key Methods:
- `SetPlayerName()` - Set player's display name
- `OnPlayerKilledEnemy()` - Increment kill count
- `OnPlayerDied()` - Increment death count
- `GetKDRatio()` - Calculate kill/death ratio

#### Key Properties:
- `PlayerName` - Player's display name
- `Kills` - Total kills by this player
- `Deaths` - Total deaths by this player
- `ControlledCharacter` - Reference to controlled character

---

### 5. ADoorActor
**Location:** `Source/MultiplayerFPS/Public/Environment/DoorActor.h/cpp`  
**Inherits From:** `AActor`  
**Purpose:** Interactive door with automatic opening/closing based on proximity

#### Key Features:
- Automatic opening when player nearby
- Automatic closing when player leaves
- Smooth rotation animation
- Overlap-based detection
- Skeletal mesh animation support

#### Key Methods:
- `OpenDoor()` - Animate door open
- `CloseDoor()` - Animate door closed
- `UpdateDoorRotation()` - Smooth door rotation
- `OnTriggerBoxOverlapBegin()` - Detect player enter
- `OnTriggerBoxOverlapEnd()` - Detect player leave
- `IsOpen()` - Query door state

#### Key Properties:
- `DoorOpenAngle` / `DoorCloseAngle` - Door rotation limits
- `DoorSpeed` - Rotation speed (degrees/second)
- `DoorDetectionRange` - Proximity trigger radius
- `CurrentDoorRotation` / `TargetDoorRotation` - Current/target angles

---

## Build & Module Files

### 1. MultiplayerFPS.Build.cs
**Purpose:** Module build configuration and dependencies  
**Contents:**
- Public dependencies: Core, CoreUObject, Engine, InputCore, EnhancedInput, UMG, Networking
- Module compilation settings
- PCH usage configuration

### 2. MultiplayerFPS.Target.cs
**Purpose:** Game executable build target configuration  
**Contents:**
- Target type: Game
- Module list
- Build settings version

### 3. MultiplayerFPSEditor.Target.cs
**Purpose:** Editor executable build target configuration  
**Contents:**
- Target type: Editor
- Module list
- Build settings version

### 4. MultiplayerFPS.uproject
**Purpose:** Unreal project file with metadata  
**Contents:**
- Engine version (5.3)
- Module definitions
- Plugin requirements (EnhancedInput, Networking)
- Target platforms (Win64, Linux, Mac)

---

## Documentation Files

### 1. BUILD_INSTRUCTIONS.md
**Purpose:** Complete guide for building and running the project  
**Contents:**
- System requirements
- Prerequisites installation
- Step-by-step build process
- Running from Visual Studio/Editor
- Packaging for distribution
- Troubleshooting guide

### 2. MIGRATION_GUIDE.md
**Purpose:** Reference for Unity developers converting to UE5  
**Contents:**
- Project structure comparison
- Class mapping (Unity → UE5)
- Key differences and patterns
- Asset conversion reference
- Networking conversion (Photon → UE5 replication)
- Common code patterns

### 3. PROJECT_STATUS.md
**Purpose:** Detailed migration progress tracking  
**Contents:**
- Overall project status
- Completed phases with deliverables
- Planned phases and timelines
- File structure created
- Known limitations
- Statistics and metrics

### 4. README.md (Updated)
**Purpose:** Project overview with migration progress  
**Contents:**
- Migration status and quick start
- Detailed 13-phase migration checklist
- Original game description (from Unity version)
- Links to build and migration guides

---

## Key Design Patterns Used

### 1. Component-Based Architecture
```cpp
// Characters have camera, movement, weapon components
UCameraComponent* FirstPersonCameraComponent;
UCharacterMovementComponent* MovementComponent;
```

### 2. Input Binding Pattern
```cpp
// Enhanced Input System for modern input handling
EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, 
    this, &AMultiplayerFPSCharacter::Move);
```

### 3. Overlap Event Pattern
```cpp
// Overlap events for collision detection
TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ADoorActor::OnTriggerBoxOverlapBegin);
```

### 4. Property Reflection Pattern
```cpp
// Expose properties to editor
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
float Damage = 25.0f;
```

### 5. Getter/Setter Pattern
```cpp
// Safe property access
UFUNCTION(BlueprintCallable, Category = "Health")
float GetHealth() const { return CurrentHealth; }
```

---

## Dependencies

| Module | Purpose |
|--------|---------|
| **CoreMinimal** | Core UE4 types and macros |
| **CoreUObject** | Base object system |
| **Engine** | Game framework and actors |
| **InputCore** | Input device definitions |
| **EnhancedInput** | Modern input system |
| **UMG** | UI widget system (prepared for Phase 5) |
| **Networking** | Network replication (prepared for Phase 4) |

---

## Class Hierarchy

```
AActor (UE5 Base)
├── ACharacter (UE5 Pawn with skeletal mesh)
│   └── AMultiplayerFPSCharacter (Player character with FPS controls)
├── AGameModeBase (UE5 Game rules)
│   └── AMultiplayerFPSGameMode (Custom game mode)
├── APlayerController (UE5 Player input)
│   └── AMultiplayerFPSPlayerController (Custom player controller)
└── ADoorActor (Custom interactive door)

AActor (UE5 Base)
└── AWeapon (Custom weapon base class)
```

---

## Final Integration Steps

1. **Finalize asset bindings:** Assign skeletal meshes, materials, and audio in UE5
2. **Validate animations:** Test montages, blend spaces, and movement states
3. **Verify multiplayer:** Confirm lobby, session, and disconnect handling
4. **Package build:** Create and test release builds for target platforms
5. **Publish documentation:** Complete release notes and deployment instructions

---

## File Statistics

| Category | Count | Lines |
|----------|-------|-------|
| Header Files (.h) | 5 | ~650 |
| Implementation (.cpp) | 5 | ~850 |
| Build Files (.cs, .uproject) | 4 | ~100 |
| Documentation | 4 | ~2000+ |
| **Total** | **18** | **~3600+** |

---

## Notes for Future Developers

1. **Enhanced Input System** - All input is configured through input actions and mapping contexts
2. **Replication Ready** - Classes are structured for networking but Phase 4 will add network properties
3. **Blueprint Extensible** - All public methods and properties can be overridden in Blueprint
4. **Performance** - Raycast-based weapons are efficient for multiplayer FPS games
5. **Modular Design** - Each system (Character, Weapon, Door, GameMode) is independent and testable

---

**Last Updated:** April 26, 2026  
**Version:** 1.0 - Initial C++ Conversion  
**Target Engine:** Unreal Engine 5.3+
