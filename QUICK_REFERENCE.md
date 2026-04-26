# Quick Reference Checklist

## Pre-Build Checklist

### System Requirements
- [ ] Unreal Engine 5.3+ installed
- [ ] Visual Studio 2022 with C++ desktop development installed
- [ ] Windows 10/11 with 100GB+ free space
- [ ] Git installed and configured

### Project Setup
- [ ] Navigate to project directory
- [ ] Right-click MultiplayerFPS.uproject
- [ ] Select "Generate Visual Studio project files"
- [ ] Wait for generation to complete (takes 1-2 minutes)

## Building the Project

### Step 1: Open Solution
- [ ] Double-click `MultiplayerFPS.sln`
- [ ] Wait for Visual Studio to load
- [ ] Verify solution loads without errors

### Step 2: Configure Build
- [ ] Set Configuration dropdown to **Development Editor**
- [ ] Set Platform dropdown to **Win64**
- [ ] Right-click **MultiplayerFPS** project → **Set as Startup Project**

### Step 3: Compile
- [ ] Press **Ctrl+Shift+B** or Build → Build Solution
- [ ] Wait for compilation to complete (takes 3-5 minutes first time)
- [ ] Verify zero errors in Output window

### Step 4: Run
- [ ] Press **F5** to launch editor
- [ ] Or use Unreal Engine Editor directly via .uproject file

## Post-Build Steps

### Verify Installation
- [ ] Editor launches without crashes
- [ ] Content browser visible
- [ ] Project settings accessible
- [ ] No console errors on startup

### Next Phase (Phase 3)
- [ ] Review [SOURCE_REFERENCE.md](SOURCE_REFERENCE.md)
- [ ] Plan animation system improvements
- [ ] Design bullet impact effects
- [ ] Begin Phase 3 implementation

## Documentation Quick Links

| Document | Purpose | Time |
|----------|---------|------|
| [BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md) | Complete build guide | 10 min read |
| [SOURCE_REFERENCE.md](SOURCE_REFERENCE.md) | Code documentation | 15 min read |
| [MIGRATION_GUIDE.md](MIGRATION_GUIDE.md) | Unity→UE5 mapping | 20 min read |
| [PROJECT_STATUS.md](PROJECT_STATUS.md) | Progress tracking | 5 min read |
| [CONVERSION_SUMMARY.md](CONVERSION_SUMMARY.md) | Overview | 10 min read |

## C++ Classes Overview

### 1. AMultiplayerFPSCharacter
**Location:** `Source/MultiplayerFPS/Public/Character/`
- Player character with FPS controls
- Movement (walk/run/sprint/aim)
- Health and damage
- Weapon equipping

### 2. AWeapon
**Location:** `Source/MultiplayerFPS/Public/Weapons/`
- Base weapon class
- Raycast firing system
- Ammo management
- Damage application

### 3. AMultiplayerFPSGameMode
**Location:** `Source/MultiplayerFPS/Public/Game/`
- Game rules and spawning
- Round management
- Player tracking

### 4. AMultiplayerFPSPlayerController
**Location:** `Source/MultiplayerFPS/Public/Game/`
- Player input coordination
- Statistics tracking (K/D)
- HUD management

### 5. ADoorActor
**Location:** `Source/MultiplayerFPS/Public/Environment/`
- Interactive door
- Auto open/close on proximity
- Smooth rotation

## Common Build Issues & Solutions

### Issue: "Cannot find -lWinMM"
**Solution:** Install Windows SDK in Visual Studio

### Issue: "Unresolved external symbol"
**Solution:** 
1. Delete Intermediate/ and Binaries/ folders
2. Regenerate project files
3. Rebuild

### Issue: "Module compilation failed"
**Solution:**
1. Close Visual Studio
2. Delete Intermediate/ folder
3. Right-click .uproject → Generate VS files
4. Rebuild

### Issue: Game crashes on startup
**Solution:** Check Saved/Logs/MultiplayerFPS.log for errors

## Development Workflow

### Daily Development
1. Open Visual Studio with `MultiplayerFPS.sln`
2. Make code changes
3. Press Ctrl+Shift+B to compile
4. F5 to test in editor
5. Commit changes to git

### Code Organization
- Public declarations in `/Public/` folder
- Implementations in `/Private/` folder
- Each system in separate subfolder
- One class per file

### Adding New Features
1. Create header file in Public/
2. Create implementation in Private/
3. Add includes to .Build.cs if needed
4. Compile and test
5. Document in comments
6. Commit to git

## Performance Tips

- Use Shipping build for final packaging
- Profile with "stat" commands in console
- Monitor frame rate with "stat unit"
- Check memory with "stat memory"

## Git Workflow

### Commit Changes
```bash
git add .
git commit -m "Phase X: [Feature description]"
git push
```

### Common Commits
- `Phase 3: Add animation montages`
- `Phase 4: Implement networking replication`
- `Fix: Weapon firing raycast`
- `Docs: Update BUILD_INSTRUCTIONS`

## Testing Checklist

### Unit Testing
- [ ] Character spawns correctly
- [ ] Weapon fires without errors
- [ ] Door opens/closes on trigger
- [ ] Health system works

### Integration Testing
- [ ] Character can equip weapon
- [ ] Character can move and aim
- [ ] Weapon damage applies correctly
- [ ] Door interacts with character

### Gameplay Testing
- [ ] Single player mode works
- [ ] Multiple actors interact correctly
- [ ] No console errors
- [ ] Performance acceptable

## Phase Progress Tracker

### Phase 1-2 ✅ COMPLETE
- [x] Project setup
- [x] Core C++ classes
- [x] Build configuration
- [x] Documentation

### Phase 3 ⏳ NEXT
- [ ] Animation system
- [ ] Bullet effects
- [ ] Movement enhancements
- [ ] Combat feedback

### Phase 4-13 📋 PLANNED
- [ ] Networking (Phase 4)
- [ ] UI/HUD (Phase 5)
- [ ] Assets (Phase 6-8)
- [ ] Multiplayer (Phase 9-10)
- [ ] Optimization (Phase 11-13)

## Important Paths

```
Project Root: c:\Users\steph\OneDrive\Documents\GitHub\Multiplayer-FPS\
Source Code:  Source/MultiplayerFPS/
Build Output: Binaries/ (game executable)
Logs:         Saved/Logs/MultiplayerFPS.log
```

## Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| Ctrl+Shift+B | Build Solution |
| F5 | Start Debug |
| Ctrl+Alt+Z | Compile & Refresh |
| Alt+P | Play in editor |

## Resources

- Official Docs: https://docs.unrealengine.com/5.3/
- C++ API: https://docs.unrealengine.com/5.3/en-US/API/
- Forums: https://forums.unrealengine.com/

---

**Last Updated:** April 26, 2026  
**Project Status:** ✅ Ready to Build  
**Current Phase:** 1-2 Complete, Phase 3 Ready
