#!/usr/bin/env python3
"""Install the DirectX 11 renderer and the 1920x1080 patch into a 4x4 Evo 2 folder.

Close the game before running. Originals are kept next to the files as *.orig,
and --restore puts them back.
"""

import argparse
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
DEFAULT_DLL = HERE.parents[1] / "Bin" / "x86" / "Release" / "R.DirectX.11.0.x86.dll"
PATCH_FILE = HERE / "Patch-4x42.txt"
EXE_NAME = "4x42.exe"
RENDERER_NAMES = ("tridx8.dll", "tridx8tl.dll")
INI_NAME = Path("system") / "metal.ini"


def load_patches(path):
    patches = []
    for raw in path.read_text(encoding="ascii").splitlines():
        line = raw.strip()
        if not line or line.startswith("#"):
            continue
        offset_text, old_text, new_text = line.split()
        old_bytes = bytes.fromhex(old_text)
        new_bytes = bytes.fromhex(new_text)
        if len(old_bytes) != len(new_bytes):
            raise ValueError(f"patch at {offset_text} changes the length")
        patches.append((int(offset_text, 16), old_bytes, new_bytes))
    return patches


def backup_once(target):
    backup = target.with_name(target.name + ".orig")
    if not backup.exists():
        shutil.copy2(target, backup)
    return backup


def patch_exe(exe_path, patches):
    data = bytearray(exe_path.read_bytes())
    changed = False
    for offset, old_bytes, new_bytes in patches:
        current = bytes(data[offset:offset + len(old_bytes)])
        if current == new_bytes:
            continue
        if current != old_bytes:
            raise RuntimeError(
                f"{EXE_NAME} at 0x{offset:08X} is neither the original nor the patched bytes; "
                "this is a different build of the game")
        data[offset:offset + len(old_bytes)] = new_bytes
        changed = True
    if changed:
        backup_once(exe_path)
        exe_path.write_bytes(bytes(data))
    return changed


def install_renderer(game_dir, dll_path):
    for name in RENDERER_NAMES:
        target = game_dir / name
        backup_once(target)
        shutil.copy2(dll_path, target)


def set_resolution(game_dir, width, height):
    ini_path = game_dir / INI_NAME
    lines = ini_path.read_bytes().split(b"\r\n")
    replacements = {b"gamePIXX=": str(width).encode(), b"gamePIXY=": str(height).encode()}
    for index, line in enumerate(lines):
        for key, value in replacements.items():
            if line.startswith(key):
                lines[index] = key + value
    ini_path.write_bytes(b"\r\n".join(lines))


def restore(game_dir):
    for name in RENDERER_NAMES + (EXE_NAME,):
        backup = game_dir / (name + ".orig")
        if backup.exists():
            shutil.copy2(backup, game_dir / name)
            print(f"restored {name}")


def parse_args(argv):
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("game_dir", type=Path, help="folder containing 4x42.exe")
    parser.add_argument("--dll", type=Path, default=DEFAULT_DLL,
                        help="built R.DirectX.11.0.x86.dll (default: the repo's Release build)")
    parser.add_argument("--width", type=int, default=1920)
    parser.add_argument("--height", type=int, default=1080)
    parser.add_argument("--skip-exe", action="store_true", help="do not patch 4x42.exe")
    parser.add_argument("--restore", action="store_true", help="put the original files back")
    return parser.parse_args(argv)


def main(argv):
    args = parse_args(argv)
    game_dir = args.game_dir.resolve()
    if not (game_dir / EXE_NAME).is_file():
        print(f"{EXE_NAME} not found in {game_dir}", file=sys.stderr)
        return 1

    if args.restore:
        restore(game_dir)
        return 0

    if not args.dll.is_file():
        print(f"renderer DLL not found: {args.dll}. Build R.DirectX.11.0 first.", file=sys.stderr)
        return 1

    if not args.skip_exe:
        try:
            patched = patch_exe(game_dir / EXE_NAME, load_patches(PATCH_FILE))
        except RuntimeError as error:
            print(error, file=sys.stderr)
            return 1
        print(f"{EXE_NAME}: {'patched' if patched else 'already patched'}")

    install_renderer(game_dir, args.dll)
    print("installed DirectX 11 renderer")

    set_resolution(game_dir, args.width, args.height)
    print(f"resolution set to {args.width}x{args.height}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
