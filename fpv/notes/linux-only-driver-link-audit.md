# Linux-only driver/link audit — 2026-11-17

Inspected tracked source in `feature/openipc-fpv`; no hardware execution.

## Confirmed LiteOS media driver entry points

`amp/a7_liteos/mpp/init/sdk_init.c`:
- `VI_init()` -> `vi_mod_init()`
- `ISP_init()` -> `isp_mod_init()`
- `VPSS_init()` -> `vpss_mod_init(NULL)`
- `VENC_init()` -> `venc_mod_init(NULL)`
- `H264E_init()` -> `h264e_mod_init()`
- `H265E_init()` -> `h265e_mod_init()`
- Also initializes `rc_mod_init()`, `chnl_mod_init()`,
  `vedu_mod_init()` and other media modules.
- Startup calls to VI, ISP, VPSS and VENC occur near lines
  974, 980, 986 and 1016 respectively; this is not a complete
  verified ordering of all encoder-related modules.

## Server build architecture

`middleware/ndk/code/mediaserver/OSTYPE.param` selects
`OSTYPE=liteos` when `MAPI_CPU_TYPE=dual` or `liteos`,
and `OSTYPE=linux` otherwise.

`middleware/ndk/code/mediaserver/Makefile.param` explicitly
includes `MPP_HEADER_LITEOS`, `OSAL_HEADER_LITEOS`,
`IPCMSG_HEADER_LITEOS` and `DATAFIFO_HEADER_LITEOS`.

`middleware/ndk/code/mediaserver/venc/Makefile` builds
`mapi_venc.c`, the chip-specific adapter, and the OS-specific
`os/$(OSCFG)/mapi_venc_os.c`. The `Make.liteos` rule builds
`libhi3559v200_mapi_venc.a` and `.so` **from MAPI server
sources**, not a native MPI implementation.

## Important distinction

`libhi3559v200_mapi_venc.a` is not `libmpi.a`.
The native `HI_MPI_VENC_*` implementation and its low-level
module objects remain to be identified.

## Next actions

1. Inspect the LiteOS firmware final link command, linker map
   and link library paths for `libmpi.a` and VENC/ISP/VI/VPSS
   objects.
2. Trace definitions of `venc_mod_init`, `vedu_mod_init`,
   `h264e_mod_init`, `h265e_mod_init`; distinguish prototypes
   from definitions and prebuilt objects.
3. Compare `MAPI_CPU_TYPE=linux` branch with the dual branch;
   confirm whether the Linux-only branch is actually supported
   for Hi3559V200, not merely shared code for other SoCs.
4. Inspect original SDK archive and firmware images; Git tree
   does not contain binary implementation artifacts.

No Linux-only encoder or hardware result has been demonstrated.
