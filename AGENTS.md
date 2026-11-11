# Unreal Engine 5.8 + MCP Project Rules for Codex
- **Required:** Use Unreal Engine MCP.
- **Required:** This is a RPG remember to follow the principles of a RPG.

## 1. Architecture & Naming Conventions
- **Naming Conventions:** Follow standard UE5 prefixes (A for AActor, U for UObject, F for Structs, E for Enums, b for Booleans).
- **Pointer Types:** Always use `TObjectPtr<T>` for UPROPERTY pointer fields in C++ class definitions.
- **Memory Safety:** Any object pointer held as a class member MUST have a `UPROPERTY()` tag to prevent unexpected GC destruction.
- **Header Optimization:** Maximize forward declarations in `.h` files (`class UCharacterMovementComponent;`). Include headers in `.cpp` files.

## 2. Performance & Blueprint Boundaries
- **Ticking:** Set `PrimaryActorTick.bCanEverTick = false` by default in constructors unless ticking is explicitly required.
- **Delegates & Events:** Favor dynamic delegates (`DECLARE_DYNAMIC_MULTICAST_DELEGATE`) over per-frame checking.
- **Blueprint Boundaries:** Write core logic in C++ and expose configurable properties to Blueprints using appropriate UPROPERTY/UFUNCTION metadata.

## 3. Unreal MCP Interaction Rules
- **State Check First:** Query editor state and target asset paths via MCP before generating modifications or spawning actors.
- **Log Diagnostics:** Fetch recent Output Log entries via MCP to diagnose crashes, unresolved references, or compilation warnings.
- **Live Coding:** Respect active Live Coding sessions—avoid issuing blocking external build commands while the editor process is connected.

## 4. File Inspection & Build Constraints
- **Scope Limits:** Do not inspect, search, or modify files inside `Intermediate/`, `Binaries/`, `Saved/`, `DerivedDataCache/`, or `.vs/`.
- **Target Source:** Focus code context on `Source/<ProjectName>/`, `Plugins/`, and project configuration files (`.uproject`).*