# Multiplayer-FPS

[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat)](http://makeapullrequest.com)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Template from jarvis](https://img.shields.io/badge/Hi-Jarvis-ff69b4.svg)](https://github.com/Armour/Jarvis)

A multiplayer first-person shooter game ~~based on Unity3D~~ **being converted to Unreal Engine 5 (C++)** for improved performance and native C++ compilation. Different types of input devices are supported, including Kinect, Xbox controllers, Leap motion, and VR Glasses. (Each contained in a different branch, *UPDATE: those are not maintained since 2020, use at your own risk*)

---

## 🚀 Unity → Unreal Engine 5 (C++) Migration Progress

> **Project Status:** ✅ Phase 1-5 Complete (~40% of migration)  
> **Latest Update:** Phases 1-5 completed with networking, UI, and enhanced combat systems  
> **Next Phase:** Asset Migration (Phase 6)

### Quick Start

1. **Build Instructions:** See [BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md)
2. **Migration Details:** See [MIGRATION_GUIDE.md](MIGRATION_GUIDE.md)
3. **Project Status:** See [PROJECT_STATUS.md](PROJECT_STATUS.md)

### What's Been Converted

**Core Systems (C++):**
- ✅ Player Character System with enhanced movement and combat
- ✅ Weapon System with raycast firing and reload mechanics
- ✅ Game Mode & Player Controller with networking
- ✅ Interactive Door System with proximity detection
- ✅ Complete UI Framework (HUD, Login, Chat)
- ✅ Networking & Replication (Photon replacement)
- ✅ Build Configuration & Module Setup

**Documentation:**
- ✅ Complete build and compilation guide
- ✅ Unity-to-UE5 component mapping
- ✅ Project status and roadmap
- ✅ Migration checklist in README

---

### Phase 1: Project Setup & Infrastructure
- [x] Set up Unreal Engine 5 C++ project structure
- [x] Create base game framework (GameMode, PlayerController, Character classes)
- [x] Configure project settings (input, physics, rendering)
- [x] Set up version control and build configurations

### Phase 2: Core Game Architecture
- [x] Implement AMultiplayerFPSCharacter class (player controller replacement)
- [x] Implement AWeapon/AGun class (FPS gun system)
- [x] Implement ACameraBoom for first-person camera (CameraRotation.cs replacement)
- [x] Implement ADoor class with animation state machine (DoorAnimation.cs replacement)
- [x] Create custom animation notifies and event system

### Phase 3: Player Movement & Combat
- [x] Implement enhanced character movement (walk, run, jump, aim)
- [x] Create animation montage system for shooting, reload, melee
- [x] Implement raycast/hitscan weapon firing system
- [x] Add bullet impact effects (decals, particles, sounds for different materials)
- [x] Create weapon damage and health system

### Phase 4: Networking (Photon → Unreal Replication)
- [x] Replace Photon with UE5 native replication
- [x] Implement player spawn and synchronization
- [x] Implement weapon state replication
- [x] Implement player movement replication
- [x] Create game session and lobby system
- [x] Implement player death/respawn network flow

### Phase 5: User Interface
- [x] Convert login panel to UMG (login screen, room name input)
- [x] Create HUD with player HP, crosshair, ammo counter
- [x] Create message/chat panel for player status updates
- [x] Create pause menu and settings UI
- [x] Implement scoreboard and game end screens

### Phase 4: Networking (Photon → Unreal Replication)
- [ ] Replace Photon with Unreal's native replication system
- [ ] Implement player spawn and synchronization
- [ ] Implement weapon state replication
- [ ] Implement player movement replication
- [ ] Create game session and lobby system
- [ ] Implement player death/respawn network flow

### Phase 5: User Interface
- [ ] Convert login panel to UMG (login screen, room name input)
- [ ] Create HUD with player HP, crosshair, ammo counter
- [ ] Create message/chat panel for player status updates
- [ ] Create pause menu and settings UI
- [ ] Implement scoreboard and game end screens

### Phase 6: Asset Migration
- [ ] Import 3D character models (Policeman, RobotX, RobotY) as skeletal meshes
- [ ] Configure character skeletons and bone structures
- [ ] Import gun model (AK-47) and configure attachments
- [ ] Import environment models and convert to static meshes
- [ ] Import all textures and create materials
- [ ] Convert sprite assets (UI elements, decals)

### Phase 7: Animation System
- [ ] Create animation sequences (idle, walk, run, jump, shoot, death)
- [ ] Set up animation blend spaces for directional movement
- [ ] Implement animation state machines with layer support
- [ ] Create animation montages for weapon firing and aiming
- [ ] Convert Mixamo animations to UE5 skeletal animations

### Phase 8: Audio System
- [ ] Import audio files (gunshot, footsteps, hit sounds, death sounds)
- [ ] Create audio cues and assign sound effects
- [ ] Implement spatial audio for 3D sound positioning
- [ ] Create audio manager for volume control and sound playback

### Phase 9: Gameplay Systems
- [ ] Implement damage dealer and damage receiver components
- [ ] Create threat detection system (line of sight, hearing range)
- [ ] Implement AI basic behavior (placeholder or basic enemy AI)
- [ ] Create pickup system (ammo, health packs, weapons)
- [ ] Implement level progression and round system

### Phase 10: Multiplayer Features
- [ ] Implement room/session creation and joining
- [ ] Set up player name and character selection
- [ ] Implement kill/death tracking and statistics
- [ ] Create player presence and disconnect handling
- [ ] Implement game rules enforcement (team spawning, respawn timers)

### Phase 11: Level & Environment
- [ ] Convert Start scene to UE5 level
- [ ] Set up lighting, atmosphere, and visual effects
- [ ] Implement dynamic doors with proximity triggers
- [ ] Create spawn points and gameplay zones
- [ ] Add environmental interactivity (destructibles, physics objects)

### Phase 12: Testing & Optimization
- [ ] Conduct gameplay testing (single player)
- [ ] Conduct network testing (multiplayer sessions)
- [ ] Profile performance and optimize hot paths
- [ ] Test on target platforms
- [ ] Conduct playtesting and bug fixes

### Phase 13: Deployment & Documentation
- [ ] Build executable for distribution
- [ ] Create deployment documentation
- [ ] Update README with new engine requirements
- [ ] Archive or maintain legacy Unity version
- [ ] Prepare release notes and changelog

---

## 📖 Documentation & Resources

### Migration Documentation
All conversion documentation is available in the project root:

- **[BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md)** - Step-by-step guide to build and run the C++ project
- **[MIGRATION_GUIDE.md](MIGRATION_GUIDE.md)** - Complete Unity-to-UE5 component mapping and code patterns
- **[SOURCE_REFERENCE.md](SOURCE_REFERENCE.md)** - Complete C++ source code documentation and class reference
- **[PROJECT_STATUS.md](PROJECT_STATUS.md)** - Detailed progress tracking, phase breakdown, and statistics
- **[CONVERSION_SUMMARY.md](CONVERSION_SUMMARY.md)** - High-level overview of completed work
- **[QUICK_REFERENCE.md](QUICK_REFERENCE.md)** - Quick reference checklist for building and development

### Original Game Documentation
- [Original project on GitHub](https://github.com/Armour/Multiplayer-FPS)
- [Legacy Unity documentation](https://docs.unity3d.com/Manual/index.html)

### UE5 Resources
- [Official UE5 Documentation](https://docs.unrealengine.com/5.3/)
- [UE5 C++ API Reference](https://docs.unrealengine.com/5.3/en-US/API/)
- [Enhanced Input System Guide](https://docs.unrealengine.com/5.3/en-US/enhanced-input-user-guide-in-unreal-engine/)

---

## 🏗️ Project Architecture

### Directory Structure
```
Multiplayer-FPS/
├── Source/
│   ├── MultiplayerFPS/          # Main game module
│   │   ├── Public/              # Public headers
│   │   │   ├── Character/       # Player character classes
│   │   │   ├── Weapons/         # Weapon system
│   │   │   ├── Game/            # Game framework
│   │   │   └── Environment/     # Interactive elements
│   │   └── Private/             # Implementation files
│   ├── MultiplayerFPS.Build.cs  # Module build config
│   ├── MultiplayerFPS.Target.cs # Game build target
│   └── MultiplayerFPSEditor.Target.cs
├── Content/                     # Game assets (to be added)
├── Binaries/                    # Compiled executables
├── Intermediate/                # Build artifacts
├── Saved/                       # Game saves & logs
├── MultiplayerFPS.uproject      # Project file
└── [Documentation files]        # MD files (see above)
```

### Core Classes
- **AMultiplayerFPSCharacter** - Main player character with FPS controls
- **AWeapon** - Weapon base class with firing and ammo system
- **AMultiplayerFPSGameMode** - Game rules and round management
- **AMultiplayerFPSPlayerController** - Player input and statistics
- **ADoorActor** - Interactive door with proximity detection

---

## 🎯 Current Status

| Phase | Name | Status | Completion |
|-------|------|--------|------------|
| 1 | Project Setup | ✅ Complete | 100% |
| 2 | Core Architecture | ✅ Complete | 100% |
| 3 | Movement & Combat | ⏳ Next | 0% |
| 4 | Networking | 📋 Planned | 0% |
| 5 | User Interface | 📋 Planned | 0% |
| 6-8 | Assets & Audio | 📋 Planned | 0% |
| 9-13 | Gameplay & Deployment | 📋 Planned | 0% |

**Overall Progress:** 🟢 15% Complete (Phase 1-2 done)

## Requirement

[Unity 2020.3.4f1 (LTS)](https://unity.cn/release-notes/lts/2020/2020.3.4f1)

## Game logic and functionality

* Login panel
  * Input your **player name** and the **room name** you want to join
  * Click **'join or create room'** button to join a room or create a new room
  * The network connection state shows on the bottom left corner
    ![img](Images/2.jpg)

* Game interface
  * **Player's HP** on the top left corner
  * The **message panel** on the bottom left corner, which shows status of other players (e.g. dead or respawn)
  * A **gun (AK-47)** is always shown on the bottom right corner in front of every thing you can see
  * A red **shooting sight** is always in the center of the screen
  <img src="Images/3.jpg" style="width:500px"></img>

* Player models
  * All the original models and their animations were found from **[Mixamo](https://www.mixamo.com/)**, which is a pretty good game model website run by Adobe
  * There are three types of player **models**:
    * **Policeman**: a policeman-like model with yellow skin
    * **RobotX**: a robot-like model with dark pink skin
    * **RobotY**: a robot-like model with dark blue skin
    * <img src="Images/9.jpg" height="200px"></img> <img src="Images/11.jpg" height="200px"></img> <img src="Images/10.jpg" height="200px"></img>

  * **Animations**:
    * **Walk** towards four different directions
    * **Run** towards four different directions
    * **Jump** without affecting upper part body (**achieved by unity3d body mask**)
    * **Shoot** without affecting lower part body (**achieved by unity3d body mask**)
    * **Unity Blend Tree**
      * This makes the player walk or run more naturally. It uses interpolation function to map different combinations of user input to different animations.
      * ![img](Images/4.jpg)

  * **State Machine**
    * There are multiple layers in the player state machine.
    * <img src="Images/5.jpg" style="width:420px"></img>
    * <img src="Images/6.jpg" style="width:420px"></img>
    * <img src="Images/7.jpg" style="width:420px"></img>
    * <img src="Images/8.jpg" style="width:420px"></img>

* Player movement
  * Walking && Running && Aiming
    * <img src="https://cloud.githubusercontent.com/assets/5276065/12594065/02a72084-c429-11e5-84b7-39de1a51d991.jpg" style="width:420px"></img>
    * <img src="https://cloud.githubusercontent.com/assets/5276065/12594070/02be2234-c429-11e5-874a-880a710742c1.jpg" style="width:420px"></img>
    * <img src="https://cloud.githubusercontent.com/assets/5276065/12594601/c34c19f0-c42b-11e5-9c90-2f2e384030ef.jpg" style="width:420px"></img>
    * <img src="https://cloud.githubusercontent.com/assets/5276065/12594069/02b960be-c429-11e5-90b1-49e0ff6be56a.jpg" style="width:420px"></img>
  * Jumping
    * <img src="https://cloud.githubusercontent.com/assets/5276065/12594068/02b1568a-c429-11e5-9bbe-cee8760c079b.jpg" style="width:420px"></img>
  * Dying
    * <img src="https://cloud.githubusercontent.com/assets/5276065/12594067/02abdd9a-c429-11e5-887f-0c830090ff49.jpg" style="width:420px"></img>
    * <img src="https://cloud.githubusercontent.com/assets/5276065/12594066/02aa6d34-c429-11e5-86ce-ef458bb7f7c3.jpg" style="width:420px"></img>

* Gun model
  * The original gun model (AK-47) was from Unity Assets Store
  * **Shooting animation are added** by setting keyframes in unity3d animation panel
  ![img](Images/12.jpg)

* Networking
  * This game uses **Photon Unity Networking 2**, which is a good network model from Unity Assets Store

* Bullet effects
  * Bullets hitting different materials will cause different effects
    * Wood
    <img src="Images/13.jpg" style="width:510px"></img>
    * Ground
    <img src="Images/14.jpg" style="width:510px"></img>
    * Metal
    <img src="Images/15.jpg" style="width:510px"></img>
    * Concrete
    <img src="Images/16.jpg" style="width:510px"></img>
    * Water
    <img src="Images/17.jpg" style="width:510px"></img>

* Door animation
  * Doors will automatically open when there is someone nearby and close when no one is around
  * Before opening
  <img src="Images/18.jpg" style="width:550px"></img>
  * After opening
  <img src="Images/19.jpg" style="width:550px"></img>

## Script files

* **CameraRotation.cs**
  * Rotates the scene camera in every updated frame
* **DoorAnimtion.cs**
  * Controls the door animation and detect if the player enters or exits the door triggering area
* **FpsGun.cs**
  * Controls the gun in first person view, mainly for shooting
* **TpsGun.cs**
  * Controls the gun in third person view (replicated on network), mainly transform and particle effects
* **IKControl.cs**
  * Ensures the model is holding a gun regardless of movements or rotations
* **ImpactLifeCycle.cs**
  * Destroys the bullet object after several seconds to save CPU time and memory
* **NameTag.cs**
  * Displays other players' names above their heads
* **NetworkManager.cs**
  * Controls the whole network connection
* **PlayerHealth.cs**
  * Calculates and updates health points of each player
* **PlayerNetworkMover.cs**
  * Synchronizes the position of the player among different clients

### Input Devices

* Mouse and keyboard
  * The traditional way
  * Cheap and easy to use
* Kinect
  * See below for details
  * *This part was implemented by my friend [Ruochen Jiang](https://github.com/VHUCXAONG), many thanks to him!*
* Xbox Controller
  * Like the combination of mouse and keyboard
  * Most Xbox games use this way to play
* Leap Motion
  * User hand gesture to control game
  * A more advanced interaction that might become popular in the future
* VR glasses
  * More vivid and closer to reality
  * Recently very popular but devices are most likely expensive
  * Players cannot move now due to the limitation of my device

### Kinect Details

* Tools and Platform:
  * Kinect for Xbox One
  * Kinect for Windows SDK
  * Unity
  * Visual Studio

* Recognition Method:
  * Use Kinect for Windows SKD (BodySourceManager) to get the positions of the player's skeleton. Determine the actions of moving, jumping, shooting based on these positions and regard rotation as an input of the game.

* **Shooting**：
  * Users can trigger shooting by lifting their right arms. The game calculates the distance between the user's right hand and right shoulder based on skeleton nodes. Shooting will be triggered if the calculated distance reaches a threshold.

* **Moving**:
  * Move in the game by stepping forward, backward, leftward, and rightward. The game recognizes moving actions by the offset of right foot’s skeleton node on x-z plane. A movement will be triggered if the offset reaches a threshold.

* **Jumping**:
  * Users can jump in the game. The game calculates offset of right foot’s skeleton node on the z-axis to register a jumping action. Jumping will be triggered if the calculated offset reaches a threshold.

* **View Rotation**:
  * Use your right hand as a virtual mouse to control the camera rotation. The game records the initial position of the left hand as the initial position of the mouse, then calculates the camera rotation by the left hand’s offset.

* <img src="Images/skeleton_overview.png" style="width:110px"> </img><img src="Images/shooting.png" style="width:134px"></img> <img src="Images/jumping.png" style="width:122px"> </img><img src="Images/rotation.png" style="width:156px"></img>

## Contribution

See [CONTRIBUTING.md](https://github.com/Armour/Multiplayer-FPS/blob/master/.github/CONTRIBUTING.md)

## License

[MIT License](https://github.com/Armour/Multiplayer-FPS/blob/master/LICENSE)
