# Unity to Unreal Engine 5 Migration Guide

This document maps Unity game components to their Unreal Engine 5 (C++) equivalents for this project.

## Project Structure Comparison

### Unity Structure
```
Assets/
├── Scripts/
│   ├── CameraRotation.cs
│   ├── DoorAnimation.cs
│   ├── FpsGun.cs
│   ├── TpsGun.cs
│   └── ...
├── Animations/
├── Audio/
├── Models/
└── Scenes/
    └── Start.unity
```

### UE5 C++ Structure
```
Source/MultiplayerFPS/
├── Public/
│   ├── Character/
│   │   └── MultiplayerFPSCharacter.h
│   ├── Weapons/
│   │   └── Weapon.h
│   ├── Game/
│   │   ├── MultiplayerFPSGameMode.h
│   │   └── MultiplayerFPSPlayerController.h
│   └── Environment/
│       └── DoorActor.h
└── Private/
    └── [.cpp implementation files]
```

## Class Mapping: Unity → Unreal Engine 5

### Camera & Perspective

| Unity | UE5 | Notes |
|-------|-----|-------|
| `CameraRotation.cs` | `UCameraComponent` | First-person camera attached to character |
| `Camera.main.Rotate()` | `AddControllerPitchInput()`, `AddControllerYawInput()` | Mouse/controller input via enhanced input system |
| `GameObject.Find()` | `AActor::FindActorByName()` | Finding actors in the world |

### Character Movement

| Unity | UE5 | Notes |
|-------|-----|-------|
| `CharacterController.Move()` | `UCharacterMovementComponent` | Built-in movement component |
| `Input.GetAxis()` | Enhanced Input System | Modern input handling |
| Animator states (Walk/Run/Jump) | Animation state machine | UE5 uses animation blueprints |
| `Rigidbody.velocity` | `FVector Velocity` | Character movement velocity |

### Combat & Weapons

| Unity | UE5 | Notes |
|-------|-----|-------|
| `FpsGun.cs` / `TpsGun.cs` | `AWeapon` class | Base weapon class with firing logic |
| `Raycasting` | `LineTraceSingleByChannel()` | Hitscan weapon implementation |
| `OnTriggerEnter()` | `OnComponentBeginOverlap` | Overlap events for collision |
| Ammo management | `CurrentAmmo`, `TotalAmmo` properties | Ammunition tracking |
| Animation triggers | Animation notifies | Event-driven animation system |

### Environment & Interaction

| Unity | UE5 | Notes |
|-------|-----|-------|
| `DoorAnimation.cs` | `ADoorActor` class | Door actor with animation |
| `OnTriggerEnter/Exit()` | Box collision trigger | Proximity detection |
| `Animator.SetTrigger()` | Animation montages | Replaces animation parameters |
| `Vector3.Lerp()` | `FMath::Lerp()` | Smooth interpolation |

### Game Management

| Unity | UE5 | Notes |
|-------|-----|-------|
| `GameManager.cs` | `AMultiplayerFPSGameMode` | Game rules and logic |
| `PlayerPrefs` | `UGameplayStatics::GetGameplayStatics()` | Game configuration |
| Spawning logic | `GetWorld()->SpawnActor<T>()` | Actor spawning |
| `OnDestroy()` | `AActor::Destroyed()` | Cleanup on actor destruction |

## Key Differences

### 1. Object Lifecycle

**Unity:**
```csharp
void Start() { } // Called on first frame
void Update() { } // Called every frame
void OnDestroy() { } // Called when object is destroyed
```

**UE5:**
```cpp
virtual void BeginPlay() override; // Called when actor enters world
virtual void Tick(float DeltaTime) override; // Called every frame
virtual void Destroyed() override; // Called when actor is destroyed
```

### 2. Component System

**Unity:**
```csharp
GetComponent<Rigidbody>().velocity = new Vector3(10, 0, 0);
GetComponent<Animator>().SetBool("isRunning", true);
```

**UE5:**
```cpp
GetCharacterMovement()->Velocity = FVector(10, 0, 0);
GetMesh()->PlayAnimation(RunAnimation, false);
```

### 3. Input System

**Unity (Old):**
```csharp
if (Input.GetKey(KeyCode.W)) { Move(); }
```

**UE5 (Enhanced Input System):**
```cpp
EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACharacter::Move);
```

### 4. Physics & Collision

**Unity:**
```csharp
Physics.Raycast(origin, direction, out hit, distance);
```

**UE5:**
```cpp
GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams);
```

### 5. Animation System

**Unity:**
- Animator with parameters (`SetFloat`, `SetBool`, `SetTrigger`)
- Animation Layers and Masks for partial body animation

**UE5:**
- Animation State Machines with transitions
- Animation Montages for special animations (firing, reloading)
- Skeletal Control for procedural animation
- Animation Notifies for triggering events

## Asset Conversion

### Models (Meshes)

| Unity Asset | UE5 Equivalent | Process |
|------------|-----------------|---------|
| `.fbx` (FBX model) | Skeletal/Static Mesh | Import with appropriate settings |
| Mesh colliders | Collision capsule/box | Configure in physics settings |
| Materials | Material Blueprint | Recreate materials in UE5 |

### Animations

| Unity Asset | UE5 Equivalent | Process |
|------------|-----------------|---------|
| `.anim` file | Animation Sequence | Import from FBX source |
| Animator controller | Animation Blueprint | Recreate state machine logic |
| Animation mask | Skeletal Control layer | Use slots and blend spaces |

### Audio

| Unity Asset | UE5 Equivalent | Process |
|------------|-----------------|---------|
| `.wav` / `.mp3` | Sound Wave | Import directly |
| Audio source | Audio Component | Attach to actor |
| Audio mixer | Sound Mix | Mix and apply dynamically |

## Networking Conversion

### Photon PUN2 → Unreal Replication

| Photon Concept | UE5 Equivalent | Implementation |
|-----------------|-----------------|-----------------|
| `[PunRPC]` | `UFUNCTION(Server/Client)` | Net function replicas |
| `OnPhotonSerializeView()` | Property replication | `replicated` property macro |
| `PhotonNetwork.Instantiate()` | `GetWorld()->SpawnActor()` | Server spawning with replication |
| `ExitGames.Client` | `FReplicationInfo` | Built-in replication system |
| Player list | `AGameState::PlayerArray` | Built-in player tracking |

## Common Conversion Patterns

### Pattern 1: Component Initialization

**Unity:**
```csharp
public void Start() {
    rb = GetComponent<Rigidbody>();
    rb.velocity = Vector3.zero;
}
```

**UE5:**
```cpp
virtual void BeginPlay() override {
    Super::BeginPlay();
    GetCharacterMovement()->Velocity = FVector::ZeroVector;
}
```

### Pattern 2: Frame Updates

**Unity:**
```csharp
public void Update() {
    if (Input.GetKey(KeyCode.W)) {
        transform.position += Vector3.forward * moveSpeed * Time.deltaTime;
    }
}
```

**UE5:**
```cpp
virtual void Tick(float DeltaTime) override {
    Super::Tick(DeltaTime);
    AddMovementInput(GetActorForwardVector(), 1.0f);
}
```

### Pattern 3: Collision/Overlap

**Unity:**
```csharp
void OnTriggerEnter(Collider other) {
    if (other.tag == "Player") {
        OpenDoor();
    }
}
```

**UE5:**
```cpp
UFUNCTION()
void OnTriggerBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, 
    AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
    bool bFromSweep, const FHitResult& SweepResult) {
    if (OtherActor && OtherActor->IsA<AMultiplayerFPSCharacter>()) {
        OpenDoor();
    }
}
```

## Development Tips

1. **Use Blueprints for rapid prototyping**, then convert to C++ for performance
2. **Use Enhanced Input System** for modern input handling (not old Axis system)
3. **Always use replication macros** for multiplayer features
4. **Test frequently** with multiplayer sessions (PIE with 2+ clients)
5. **Profile performance** using built-in profiling tools (Stat commands)
6. **Use proper class naming conventions** (A for Actors, U for Objects, F for structures)

## Reference Links

- [UE5 C++ Quick Start](https://docs.unrealengine.com/5.3/en-US/quick-start-for-cpp-development/)
- [Class Design Guidelines](https://docs.unrealengine.com/5.3/en-US/recommended-practices-for-blueprints-and-c-/)
- [Networking Architecture](https://docs.unrealengine.com/5.3/en-US/unreal-engine-networking-overview/)
- [Enhanced Input System](https://docs.unrealengine.com/5.3/en-US/enhanced-input-user-guide-in-unreal-engine/)
