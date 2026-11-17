# Tracking HI_MPI_VENC implementation — Hi3559V200

Date: 2026-11-17. Static source inspection only.

## Confirmed source paths

- `amp/a7_liteos/mpp/init/sdk_init.c` directly programs SoC
  pinmux, clocks, VI online/offline workmode, DDR configuration,
  and sensor-related clocks. `sensor_config()` explicitly handles
  `imx458` together with other Sony sensors.
- `amp/a7_linux/mpp/init/sys_init.c` is a Linux platform driver
  which maps CRG, SYS, DDR, MISC resources and calls `SYS_ModInit()`.
  It does not initialize the VENC pipeline.
- `amp/a7_linux/mpp/ko/load3559v200` does not load a VENC kernel
  module.
- Linux MPP public `hi_comm_venc.h` is a data-type/interface header;
  it is not the implementation of `HI_MPI_VENC_*`.

## Corroborating external references

- https://icode.best/i/19634032210186 — Hi3559V200 + IMX458
  experiment: Linux IPCMSG client, LiteOS media server executing
  sensor/VI/VPSS MPI calls.
- https://iopenv.com/3E4SUJFUR/Haisi-3559-is-compatible-with-UVC-cameras-III-and-data-is-sent-to-VPSS
  — describes LiteOS VPSS/VENC ownership and Linux↔LiteOS DATAFIFO.
- https://github.com/OpenIPC/openhisilicon — open replacement
  `libmpi_neo.so` currently covers SYS/MMZ operations, **not**
  an all-platform VENC replacement.

## Important distinction

We have located the hardware initialization **surrounding** the
media pipeline but have **not located a Linux-native implementation**
of `HI_MPI_VENC_CreateChn`, `HI_MPI_VENC_StartRecvFrame`, or
`HI_MPI_VENC_GetStream` for this SoC. This is not proof that no
binary implementation exists.

## Next investigation

1. Search full SDK artifacts for `libmpi*.a`, `libmpi*.so`,
   `*venc*.ko`, `*venc*.a`, and `mpi_venc.h`; record ELF
   architecture and symbols with `file`, `readelf`, `nm`.
2. Inspect LiteOS `sdk_init.c` remainder for module init order
   and identify VENC driver registration entry points.
3. Trace `HI_MPI_VENC_*` callsites and link rules to distinguish
   LiteOS native libraries from Linux IPC clients.
4. Confirm what is included in actual camera filesystem dump;
   absence in source tree is not sufficient evidence.
5. Do not write to MMIO or disable LiteOS until a RAM-only
   recovery/test plan exists.

## Current verdict

The **first-pass evidence supports AMP media ownership**. A direct
Linux-only VENC path remains unverified; driver reverse engineering
may be necessary, but it is premature to declare it unavoidable.
