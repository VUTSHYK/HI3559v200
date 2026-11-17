# Hi3559V200 OpenIPC FPV port

Experimental port workspace for a Hi3559V200 camera with Sony IMX458.

## Scope and safety

- Work in `feature/openipc-fpv`; preserve `master` and the stock SPI NOR dump.
- Do **not** flash images or overwrite U-Boot, environment, or calibration data until the memory map and recovery procedure are independently validated.
- The first milestone is a **host-side SDK media/VENC test** on the existing Linux + LiteOS system, not a replacement boot image.
- IMX458 presence in the SDK does not establish the correct board configuration, RAM size, sensor clock, MIPI lane mapping, or GPIO wiring for the actual camera.

## Verified SDK entry points

- `middleware/media_adpt/hi3559v200/media_init.c`: MAPI initialization and sensor/VCAP/VPROC/VENC setup.
- `middleware/media_adpt/hi3559v200/venc_ops.c`: `HI_MW_VEnc_Start` registers `VEncDataProc` as a callback.
- `middleware/media_adpt/include/hi_mw_media_intf.h`: callback and packet structures.
- `middleware/media_adpt/hi3559v200/medialibs.mak`: Linux host MAPI client libraries and IPC/datafifo dependencies.
- `middleware/sample/mw_sample/`, `middleware/sample/livestream/`, `middleware/sample/rtspserver/`: candidate reference applications, not yet verified as buildable on the target.

## Candidate IMX458 SDK configurations

- `configs/hi3559v200_actioncam_demb_imx458_256M_config`
- `configs/hi3559v200_actioncam_demb_imx458_256M_16bit_config`
- `configs/hi3559v200_actioncam_demb_imx458_512M_config`
- `configs/hi3559v200_actioncam_refb_imx458_256M_config`
- `configs/hi3559v200_actioncam_refb_imx458_256M_16bit_config`
- `configs/hi3559v200_actioncam_refb_imx458_GYRO_256M_config`
- `configs/hi3559v200_actioncam_refb_imx458_GYRO_512M_config`

The default `.config` currently selects `CONFIG_SNS0_IMX377=y`; do not modify it until the camera's actual hardware is matched.

## Milestones

1. Inventory SDK toolchain, Linux/LiteOS build prerequisites, existing media samples, and sensor modes.
2. Compare the factory boot log, flash layout, RAM map, and sensor wiring with SDK configurations.
3. Build a standalone media test from the SDK's own sample with an H.264 callback and basic statistics.
4. Run on the original system without flashing; validate SPS/PPS, timestamps, FPS, and packet lifetime.
5. Implement a bounded, nonblocking VENC-to-FPV transport adapter.
6. Integrate Buildroot/OpenIPC and Waybeam after the existing media path works.

## Open questions

- Exact board model, RAM size, MIPI lane configuration, sensor GPIO/I2C addresses, and firmware partition map.
- Whether the supplied toolchain and prebuilt media libraries are complete and compatible with the stock image.
- LiteOS media server startup sequence and the ownership of VI/ISP/VENC.
- Video packet header framing and callback lifetime for safe asynchronous forwarding.

No hardware build or runtime test has been performed yet.
