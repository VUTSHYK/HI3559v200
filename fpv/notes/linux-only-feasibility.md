# Linux-only media feasibility — Hi3559V200

Initial static repository inspection, 2026-11-17. **No hardware tests.**

## Evidence

- `amp/a7_linux/mpp/` contains `component/hifb`, `init/`, `ko/load3559v200`, and public headers such as `hi_comm_venc.h`, `hi_comm_isp.h`, `hi_comm_vi.h`, `hi_comm_vpss.h`.
- `amp/a7_linux/mpp/ko/load3559v200` inserts OSAL, base, sys, TDE, ADC, hifb, IVE, NNIE; it does **not** insert VENC, VI, VPSS or ISP media modules.
- `amp/a7_liteos/mpp/component/` contains `isp` and `pm`; Linux component directory only lists `hifb`.
- The Linux MPP makefiles alone do not prove the existence of Linux VENC driver binaries or a working MPI implementation. Header availability does not establish runtime support.

## Interpretation

The SDK is organized around an asymmetric AMP media architecture.
A standalone Linux VENC pipeline is **not established** by the inspected
files. It may be possible with additional vendor binaries/driver sources,
but this cannot be claimed yet.

## Linux-only research gates

1. Inventory all VENC/VI/VPSS/ISP Linux `.ko`, `.a`, `.so` and source
   files in SDK and stock filesystem; distinguish headers from implementations.
2. Inspect linker dependencies for `HI_MPI_VENC_*` on Linux; check
   whether functions are native or IPC shims.
3. Check Linux kernel media/clock/reset/MMZ ownership and ARM core
   memory map; ensure no conflict with LiteOS reserved memory.
4. If a Linux-native driver exists, prototype a **read-only** discovery
   program and then a single-channel VENC smoke test in RAM.
5. If it does not, scope a separate driver reverse-engineering project:
   register documentation, firmware dependencies, IRQ/DMA/MMZ,
   ISP sensor bring-up, and userspace ABI. This is high risk and
   cannot be assumed achievable quickly.

## Safety

Do not disable LiteOS or flash Linux-only firmware until a reversible
RAM-boot experiment is available and the original bootloader, partitions,
calibration and sensor data are backed up.
