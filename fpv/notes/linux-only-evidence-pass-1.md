# Linux-only feasibility: evidence pass 1 (2026-11-17)

## Direct SDK observations

- `amp/a7_linux/mpp/init/`: `base_init.c`, `ive_init.c`,
  `nnie_init.c`, `sys_init.c`, `tde_init.c`. No matching
  `venc_init.c`, `vi_init.c`, `vpss_init.c`, `isp_init.c`.
- `amp/a7_liteos/mpp/init/`: `sdk_init.c`, `sdk_exit.c`.
- `amp/a7_linux/mpp/component/`: only `hifb`.
- `amp/a7_liteos/mpp/component/`: `isp`, `pm`.
- `amp/a7_linux/mpp/ko/load3559v200`: inserts OSAL, base, sys,
  TDE, ADC, hifb, IVE and NNIE; no media pipeline drivers.
- Linux `hi_comm_venc.h` is an interface header, not proof of a
  Linux VENC implementation.
- The upstream SDK default `.config` selects a Linux+LiteOS
  memory partition and IMX377, not the target board IMX458.

## Independent corroboration

- Hi3559V200 brief datasheet explicitly describes a
  `Linux+Huawei LiteOS dual-system solution`:
  https://www.silicondevice.com/file.upload/images/Gid1570Pdf_Hi3559%20V200%204K%20Mobile%20Camera%20SoC%20Brief%20Data%20Sheet.pdf
- An external Hi3559/IMX458 engineering writeup says VPSS/VENC
  operations run on LiteOS and Linux uses IPC/DATAFIFO:
  https://icode.best/i/19634032210186
  and https://iopenv.com/3E4SUJFUR/Haisi-3559-is-compatible-with-UVC-cameras-III-and-data-is-sent-to-VPSS
  This is independent experience, not proof for our exact firmware.
- Do not confuse Hi3559**A**V100 (which advertises Linux SMP)
  with Hi3559**V200**.

## Working conclusion

The available evidence favors the vendor AMP media ownership model.
There is **no demonstrated Linux-native ISP/VI/VPSS/VENC pipeline** in
the inspected tree. Linux-only is still an investigation, not an
implementation-ready port.

## Next concrete evidence collection

1. Inventory actual `.ko`, `.a`, `.so` artifacts, symlinks and
   symbols in both SDK and camera rootfs; check ELF architectures.
2. Trace `HI_MPI_VENC_*` definition and linking, and the LiteOS
   `sdk_init.c` module initialization sequence.
3. Check whether vendor Linux-side MPI names resolve to local
   drivers or IPC clients.
4. If no Linux VENC implementation is found, document driver
   requirements (MMIO, clocks, reset, IRQ, MMZ, firmware, sensor
   ISP pipeline) before deciding to port or reverse engineer.

Do not flash, disable LiteOS or change boot memory allocation.
