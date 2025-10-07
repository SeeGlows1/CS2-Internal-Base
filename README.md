## Internal

Lightweight Windows C++ project showcasing an internal overlay with DirectX 11 rendering and an ImGui-based UI, using function hooking via MinHook. This repository is intended for learning, experimentation, and research in rendering overlays and runtime instrumentation on Windows.

> Important: This project is provided for educational purposes only.

### Features
- **ImGui UI**: Dear ImGui integration for a configurable in-app menu (`external/imgui`, `src/core/menu.*`).
- **DirectX 11 backend**: Swap chain hooking abstractions (`src/sdk/interfaces/swap_chain_dx11.hpp`).
- **Hooking via MinHook**: Minimal, battle-tested inline hooking library (`external/minhook`).
- **Modular architecture**: Core systems and feature modules separated under `src/`.
- **Example visuals module**: ESP/visuals feature skeleton (`src/features/esp/`).

### Project structure
```
build/                     # Compiled artifacts (x64): Injector-x64.exe, Internal.dll
external/
  imgui/                   # Dear ImGui and DX11/Win32 backends
  minhook/                 # MinHook source
src/
  core/                    # Hooks, interfaces, globals, menu
  features/esp/            # Visuals/ESP module
  math/                    # Math utilities
  sdk/                     # SDK abstractions (input, swap chain, memory utils)
  dll_main.cpp             # DLL entry and initialization
Internal.sln               # Visual Studio solution
Internal.vcxproj           # MSVC project
```

### Getting started
#### Prerequisites
- Windows 10 or later (x64)
- Visual Studio 2022 (v143 toolset)
- Desktop development with C++ workload

#### Build (Visual Studio)
1. Open `Internal.sln` in Visual Studio.
2. Set solution configuration to `Release` and platform to `x64`.
3. Build the solution. Artifacts will appear under `build/` and intermediates under `Internalintermediates/`.

Notes:
- Third-party sources (`external/imgui`, `external/minhook`) are vendored; no extra package manager steps are required.
- If you hit missing Windows SDK errors, install the latest Windows 10/11 SDK via the VS Installer and retarget the solution.

### Usage
This repository demonstrates techniques for UI rendering and API hooking in a Windows process for research and educational exploration. It intentionally omits any step-by-step instructions that could facilitate misuse. Operate only in controlled environments where you own the software and have permission to test.

### Development notes
- Code style: Prefer clear, descriptive identifiers and early returns; avoid deep nesting.
- Toolchain: MSVC, C++17 or later recommended.
- External libraries: Dear ImGui (MIT), MinHook (BSD-2-Clause).

### Troubleshooting
- "Cannot open include file" or SDK errors: Retarget to an installed Windows SDK (Project → Properties → General → Windows SDK Version).
- LNK2038/LNK2019: Ensure platform is `x64` for all projects and dependencies; clean/rebuild.
- DX11 headers not found: Install the Windows SDK via Visual Studio Installer.

### Credits
- [Dear ImGui](https://github.com/ocornut/imgui)
- [MinHook](https://github.com/TsudaKageyu/minhook)

### License
No license file is currently included. If you intend to open-source this, add a `LICENSE` file (e.g., MIT/BSD-2-Clause) and update this section. Until a license is added, all rights are reserved by the repository owner.

### Disclaimer
This codebase is for educational and research purposes. You are solely responsible for complying with all applicable laws, terms, and agreements. The authors and contributors are not liable for any misuse.


