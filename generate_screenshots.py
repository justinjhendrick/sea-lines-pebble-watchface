#!/usr/bin/env python3
from pathlib import Path
import sys
import json
from subprocess import check_call

THIS_DIR = Path(__file__).absolute().parent

def main() -> int:
    with open(THIS_DIR / "package.json", mode="r") as f:
        package = json.load(f)
    check_call(["rebble", "build"])
    for platform in package["pebble"]["targetPlatforms"]:
        check_call(["rebble", "wipe"])
        check_call(["rebble", "kill"])
        check_call(["rebble", "install", "--emulator", platform])
        check_call(["rebble", "screenshot", f"screenshot_{platform}.png"])
    check_call(["rebble", "wipe"])
    check_call(["rebble", "kill"])
    return 0

if __name__ == "__main__":
    sys.exit(main())
