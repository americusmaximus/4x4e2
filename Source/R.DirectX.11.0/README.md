# DirectX 11 renderer for 4x4 Evo 2

A 32-bit DirectX 11 renderer module for the original 4x4 Evo 2 executable. It implements the same renderer interface as the DirectX 8 module, so the game loads it in place of the DX8 DLLs.

## Build

Requirements:

- Visual Studio 2022 (or newer) with the **Desktop development with C++** workload and the Windows 10/11 SDK.
- The `v143` platform toolset. Newer Visual Studio releases install it alongside their own toolset.

From the repository root (the folder that contains `4x4e2.sln`):

```
msbuild Source\R.DirectX.11.0\R.DirectX.11.0.vcxproj /p:Configuration=Release /p:Platform=Win32 /p:SolutionDir=%CD%\
```

The output is `Bin\x86\Release\R.DirectX.11.0.x86.dll`. Pass `SolutionDir` so the output lands in the repository's `Bin` folder.

Building the whole solution also builds the DX8 projects, which need the legacy DirectX 8 headers. Build this project on its own, as above.

## Install

Close the game, then run the patcher with the game folder (the one that contains `4x42.exe`):

```
python Source\R.DirectX.11.0\patch_game.py "C:\path\to\4x4evo2"
```

The patcher does three things:

1. Applies the six byte patches listed in `Patch-4x42.txt` to `4x42.exe`. These skip the startup videos, remove the swap-space check, and allow 1920x1080. Each patch is checked against the original bytes first; a different build of the game is refused.
2. Copies the built DLL over `tridx8.dll` and `tridx8tl.dll`, keeping the originals as `*.orig`.
3. Sets `gamePIXX` and `gamePIXY` in `system\metal.ini` to 1920 and 1080.

Options:

- `--width` and `--height` set a different resolution.
- `--dll` points at a DLL built somewhere else.
- `--skip-exe` leaves `4x42.exe` untouched.
- `--restore` puts `4x42.exe`, `tridx8.dll` and `tridx8tl.dll` back from their `*.orig` copies. It does not reset `metal.ini`.

The patcher only changes files inside the game folder you give it. The original game files are not needed in the repository.

## Status

Working:

- Menus and gameplay at the window's resolution. The game lays out its screen as 4:3 at the window height, and the renderer stretches it horizontally for wider windows.
- Textures, alpha test, alpha and additive blending, depth test and write, clamp modes.
- Perspective UV (`U0x400`).
- Fog colour (`SetFog`) with the per-vertex fog factor carried in specular alpha, applied when the fog mode bit (`U0x8`) is set. This matches the DX8 renderer's pre-transformed vertex path. Fog start and end distances are not used, because pre-transformed vertices take their factor from specular alpha. Fog has not been checked visually in a fogged level yet.

Not implemented (these report failure, like the DX8 renderer does when it cannot do the work):

- Render-to-texture and cube maps.
- Colour lookup tables (`SetColorTable`).
- Direct 3D TL dot and line lists (`DotListD3DTL`, `LineListD3DTL`).
- Texture and video memory statistics.
- Multi-stage blend operations (`SetMultiTextureBlend`). The DX8 renderer also rejects triangle lists when one is set, and the game does not set one in the runs tested, so this path is unused in practice.

Known differences from DX8:

- Perspective UV scales each vertex by its own `Unk15`. The DX8 renderer uses the first vertex's value for every vertex, which looks like a bug, so it is not copied.
- The software `LockFrame` surface is copied 1:1 to the back buffer. It matches the back buffer when the window is the game's resolution, which is the supported configuration.

## Logs

The renderer writes `dx11-trace.log` in the game folder. It records the renderer calls the game makes and any shader compile errors. Check it first when something is wrong.
