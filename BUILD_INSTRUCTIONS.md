# Multiplayer FPS - Build Instructions

This document provides step-by-step instructions for building and running the Unreal Engine 5 C++ version of Multiplayer FPS.

## System Requirements

- **Unreal Engine 5.3+** (with C++ development tools)
- **Visual Studio 2022** or higher (with C++ Desktop Development)
- **Windows 10/11** with 100GB+ free disk space
- **4GB+ RAM** (8GB+ recommended)
- **NVIDIA or AMD GPU** with 4GB+ VRAM (recommended)

## Prerequisites Installation

### 1. Install Unreal Engine 5.3+

1. Download and install the **Epic Games Launcher**
2. Install **Unreal Engine 5.3** (or later)
3. During installation, ensure the following components are selected:
   - Unreal Engine source code
   - Visual C++ runtime components
   - Development tools

### 2. Install Visual Studio 2022

1. Download **Visual Studio Community 2022** (or Professional/Enterprise)
2. Run the installer and select **Desktop development with C++**
3. Include the following workloads:
   - C++ desktop development
   - Game development with C++
4. Complete the installation

### 3. Install Git

1. Download and install **Git for Windows** from https://git-scm.com/
2. Configure Git with your credentials:
   ```bash
   git config --global user.name "Your Name"
   git config --global user.email "your.email@example.com"
   ```

## Building the Project

### Step 1: Clone or Navigate to Project

```bash
# Navigate to the project directory
cd c:\Users\steph\OneDrive\Documents\GitHub\Multiplayer-FPS
```

### Step 2: Generate Visual Studio Project Files

```bash
# Right-click on MultiplayerFPS.uproject and select "Generate Visual Studio project files"
# Or run from command line:
"C:\Program Files\Epic Games\UE_5.3\Engine\Build\BatchFiles\GenerateProjectFiles.bat" MultiplayerFPS.uproject
```

### Step 3: Open in Visual Studio

```bash
# Open the generated Visual Studio solution
start MultiplayerFPS.sln
```

Or:
1. Double-click `MultiplayerFPS.sln` in the project directory
2. Visual Studio will launch and load the solution

### Step 4: Set Build Configuration

In Visual Studio:
1. Set the configuration to **Development Editor**
2. Set the platform to **Win64**
3. Right-click on the **MultiplayerFPS** project → Set as Startup Project

### Step 5: Compile the Project

```
In Visual Studio, press Ctrl+Shift+B or use Build > Build Solution
```

Or from command line:
```bash
# Using MSBuild
msbuild MultiplayerFPS.sln /p:Configuration=Development /p:Platform=Win64

# Or using Unreal Automation Tool
"C:\Program Files\Epic Games\UE_5.3\Engine\Build\BatchFiles\RunUAT.bat" BuildProject -Project="$(pwd)\MultiplayerFPS.uproject" -TargetPlatforms=Win64 -ClientConfig=Development
```

### Step 6: Run the Game

After successful compilation:

**Option A: From Visual Studio**
- Press F5 or use Debug > Start Debugging

**Option B: From Unreal Engine Editor**
1. Open **Unreal Engine Editor**
2. File > Open Project
3. Navigate to `MultiplayerFPS.uproject` and select it
4. Click **Open**
5. Press Play (Alt+P) or click the Play button in the toolbar

**Option C: From Command Line**
```bash
# Open editor
"C:\Program Files\Epic Games\UE_5.3\Engine\Binaries\Win64\UE4Editor.exe" "$(pwd)\MultiplayerFPS.uproject"

# Package game for distribution
"C:\Program Files\Epic Games\UE_5.3\Engine\Build\BatchFiles\RunUAT.bat" BuildProject -Project="$(pwd)\MultiplayerFPS.uproject" -TargetPlatforms=Win64 -ClientConfig=Shipping
```

## Project Structure

```
MultiplayerFPS/
├── Source/
│   ├── MultiplayerFPS/
│   │   ├── Public/
│   │   │   ├── Character/         # Player character classes
│   │   │   ├── Weapons/           # Weapon and gun classes
│   │   │   ├── Game/              # Game mode and controller
│   │   │   └── Environment/       # Door, pickups, etc.
│   │   └── Private/
│   │       └── [Implementation files]
│   ├── MultiplayerFPS.Build.cs    # Module build configuration
│   ├── MultiplayerFPS.Target.cs   # Game build target
│   └── MultiplayerFPSEditor.Target.cs  # Editor build target
├── Content/                        # Game assets (blueprints, materials, meshes)
├── Binaries/                       # Compiled binaries
├── Intermediate/                   # Intermediate build files
├── Saved/                          # Game saves and logs
└── MultiplayerFPS.uproject        # Project file
```

## Core C++ Classes

### Character System
- **AMultiplayerFPSCharacter** - Main player character with movement and combat
- **AMultiplayerFPSPlayerController** - Player controller handling input and HUD

### Game Framework
- **AMultiplayerFPSGameMode** - Game rules, spawning, and round management

### Combat System
- **AWeapon** - Base weapon class with firing and ammo management

### Environment
- **ADoorActor** - Interactive door that opens when players are nearby

## Troubleshooting

### Compilation Errors

**Error: "Cannot find -lWinMM"**
- Solution: Install Windows SDK in Visual Studio

**Error: "Unresolved external symbol"**
- Solution: Clean and rebuild the project (Ctrl+Alt+Delete in Visual Studio)

**Error: "Module compilation failed"**
- Solution: Delete the `Intermediate` and `Binaries` folders and regenerate project files

### Runtime Issues

**Game crashes on startup**
- Check the log file: `Saved/Logs/MultiplayerFPS.log`
- Ensure all required plugins are enabled in `MultiplayerFPS.uproject`

**No input response**
- Verify Enhanced Input plugin is enabled
- Check input mapping context is properly set in the character class

## Next Steps

1. **Configure Input System**: Set up keyboard, mouse, and controller bindings
2. **Import Assets**: Add 3D models, textures, and animations
3. **Implement Networking**: Set up multiplayer synchronization
4. **Create Level**: Design game map and place spawning points
5. **Add UI**: Create main menu and in-game HUD

## Additional Resources

- [Unreal Engine Documentation](https://docs.unrealengine.com/)
- [Unreal Engine C++ API Reference](https://docs.unrealengine.com/en-US/API/)
- [Enhanced Input System Guide](https://docs.unrealengine.com/5.3/en-US/enhanced-input-user-guide-in-unreal-engine/)
- [Replication Graph for Networking](https://docs.unrealengine.com/5.3/en-US/understanding-the-replication-graph/)

## Support

For issues or questions:
1. Check the [Unreal Engine Community Forums](https://forums.unrealengine.com/)
2. Search [AnswerHub](https://www.unrealengine.com/en-US/community)
3. Review the project's GitHub issues and documentation
