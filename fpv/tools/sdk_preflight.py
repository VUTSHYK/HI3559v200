#!/usr/bin/env python3
"""Read-only preflight for the Hi3559V200 FPV SDK checkout.

Run from repository root: python3 fpv/tools/sdk_preflight.py
No build, flashing, SDK modifications, or third-party dependencies.
"""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
CONFIGS = ROOT / "configs"
SAMPLE = ROOT / "middleware/sample/mw_sample/venc_wrapper.c"
REQUIRED = [
    "amp/a7_linux/mpp/include",
    "amp/a7_liteos/mpp/include",
    "middleware/media_adpt/hi3559v200/venc_ops.c",
    "middleware/media_adpt/hi3559v200/media_init.c",
    "middleware/media_adpt/hi3559v200/medialibs.mak",
    "middleware/sample/mw_sample/Make.Linux",
    "middleware/sample/mw_sample/venc_wrapper.c",
]

def main():
    errors = 0
    print("Hi3559V200 FPV SDK preflight (read-only)")
    for rel in REQUIRED:
        exists = (ROOT / rel).exists()
        print(f"{'OK' if exists else 'MISSING'}  {rel}")
        errors += not exists

    candidates = sorted(CONFIGS.glob("hi3559v200*imx458*config"))
    print(f"IMX458 config candidates: {len(candidates)}")
    for candidate in candidates:
        text = candidate.read_text(encoding="utf-8", errors="replace")
        def setting(name):
            match = re.search(rf"^CONFIG_{name}=(.*)$", text, re.MULTILINE)
            return match.group(1) if match else "not set"
        print(f"  {candidate.name}: RAM={setting('MEM_TOTAL_SIZE')}, "
              f"LiteOS={setting('MEM_HUAWEILITE_SYS_BASE')}, "
              f"Linux={setting('MEM_LINUX_SYS_BASE')}")
    if not candidates:
        errors += 1

    if SAMPLE.exists():
        text = SAMPLE.read_text(encoding="utf-8", errors="replace")
        for sensor in ("IMX458", "IMX377", "IMX477", "IMX307"):
            present = f"SENSOR_{sensor}" in text
            print(f"mw_sample profile SENSOR_{sensor}: {'present' if present else 'absent'}")
        if "SENSOR_IMX458" not in text:
            print("NOTE: mw_sample has no SENSOR_IMX458 profile; do not build/run it "
                  "for IMX458 without a validated sensor/media configuration.")
    print(f"Preflight: {'PASS' if not errors else f'{errors} missing prerequisites'}")
    return int(bool(errors))

if __name__ == "__main__":
    sys.exit(main())
