# IMX458 sensor-mode audit — source-grounded

This note documents what is present in the fork's SDK. It **does not**
establish the actual camera board's wiring, sensor clock, or memory capacity.

## SDK sensor implementation

- `amp/a7_liteos/mpp/component/isp/user/sensor/hi3559v200/sony_imx458/imx458_cmos.c`
  defines `g_astImx458ModeTbl` and the sensor mode selection.
- `imx458_cmos_priv.h` defines `IMX458_RES_MODE_E` and its mode IDs.
- `imx458_sensor_ctl.c` contains register programming sequences.
- `configs/hi3559v200_actioncam_demb_imx458_256M_config` enables
  `CONFIG_SNS0_IMX458=y`, `CONFIG_HI3559V200=y`, and
  `CONFIG_MEM_TOTAL_SIZE="256"`. This is a DEMB board configuration,
  **not** proof of compatibility with the target camera.

## Modes declared by the SDK

| Enum | Mode label | SDK comment / nominal rate |
| --- | --- | --- |
| IMX458_8M_30FPS_10BIT_LINEAR_MODE | 4K2K_10BIT_30FPS | 3840×2160, 30 |
| IMX458_12M_20FPS_10BIT_LINEAR_MODE | 4K3K_10BIT_20FPS | 4000×3000, 19.98 |
| IMX458_4M_60FPS_10BIT_LINEAR_MODE | 2.7K1.5K_10BIT_60FPS | 2716×1524, 59.94 |
| IMX458_4M_40FPS_10BIT_LINEAR_MODE | 2.7K1.5K_10BIT_40FPS | 2716×1524, 40.62 |
| IMX458_2M_90FPS_10BIT_LINEAR_MODE | 2K1K_10BIT_90FPS | 1080p, 90.11 |
| IMX458_1M_129FPS_10BIT_LINEAR_MODE | 720P_10BIT_129FPS | 1280×720, 128.8 |
| IMX458_8M_30FPS_10BIT_LINEAR_SCALE_MODE | 4K2K_10BIT_30FPS_SCALE | 3840×2160, 30 |
| IMX458_8M_30FPS_10BIT_LINEAR_804M_MODE | 4K2K_10BIT_30FPS_804M | 3840×2160, 30 |

These are *sensor* modes, not verified encoded frame rates, bandwidth, or latency.

## Blockers before modifying mw_sample

1. `middleware/cfg.mak` specifies `CFG_CHIP_TYPE=hi3559v200` and
   `CFG_SDK_TOOLCHAIN=arm-himix100-linux-`.
2. `middleware/sample/mw_sample/Makefile` has no `hi3559v200` branch,
   so it defaults to `SENSOR_IMX477`.
3. `middleware/sample/mw_sample/venc_wrapper.c` has no
   `SENSOR_IMX458` branch. Adding a macro alone will not create the
   required sensor, VCAP, VPROC and VENC attribute definitions.
4. Confirm actual board RAM, LiteOS/Linux MMZ split, MIPI lanes, sensor
   clock/reset/I2C, media channel topology and stock encoder settings.
5. Confirm the sensor mode supported by the actual LiteOS media server.
   Do not infer that every mode in the sensor driver is usable by the
   current camera firmware.

## Suggested initial validation

First match the **stock working mode** (known to encode 4K25 H.264) rather
than assuming the SDK's 4K30 setting will work. Once the board-specific
parameters are confirmed, introduce an explicit IMX458 profile and a
single-channel H.264 callback probe. Do not flash or overwrite the stock
firmware to perform this audit.
