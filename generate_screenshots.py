#!/usr/bin/env python3
from pathlib import Path
import sys
import json
from subprocess import check_call

THIS_DIR = Path(__file__).absolute().parent

def main() -> int:
    with open(THIS_DIR / "package.json", mode="r") as f:
        package = json.load(f)
    check_call(["pebble", "build"])
    for platform in package["pebble"]["targetPlatforms"]:
        check_call(["pebble", "wipe"])
        check_call(["pebble", "kill"])
        check_call(["pebble", "install", "--emulator", platform])
        if platform in ("aplite", "flint"):
            continue  # skip screenshot because identical to diorite
        check_call(["pebble", "screenshot", f"screenshot_{platform}.png"])
    check_call(["pebble", "wipe"])
    check_call(["pebble", "kill"])
    return 0

if __name__ == "__main__":
    sys.exit(main())
