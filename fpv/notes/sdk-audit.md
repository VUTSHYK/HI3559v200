# Initial SDK audit

Date: 2026-11-17

## Observations from repository source

1. Root `.config` selects `CONFIG_HI3559V200=y` and `CONFIG_SNS0_IMX377=y`, with IMX458 available but not selected.
2. `configs/` provides several Hi3559V200/IMX458 board profiles with 256M/512M and 16-bit variants.
3. `middleware/media_adpt/hi3559v200/venc_ops.c` calls `HI_MAPI_VENC_RegisterCallback` and `HI_MAPI_VENC_Start`. Its `VEncDataProc` converts MAPI packets into `HI_MW_MEDIA_VIDEO_S` and invokes registered callbacks.
4. The callback packet can contain multiple segments (`u32SegCnt`) and exposes `u64PTS`; consumers must respect segment lengths and SDK-defined lifetime.
5. `middleware/media_adpt/hi3559v200/medialibs.mak` links `hi3559v200_venc_client`, `hi3559v200_vcap_client`, `hi3559v200_vproc_client`, `ipcmsg_a7_linux`, and `datafifo_a7_linux`.
6. `middleware/media_adpt/hi3559v200/common_ipcmsg_client.c` uses `HI_IPCMSG_CONNECT_S` with `u32RemoteId = 1`.
7. `middleware/media_adpt/hi3559v200/Makefile` includes both `amp/a7_linux/mpp/include` and `amp/a7_liteos/mpp/include`, indicating the dual-system SDK integration.
8. `middleware/ndk/readme.txt` describes multi-core and dual-system examples for **Hi3559AV100**. These examples must **not** be treated as proof of a Linux-only Hi3559V200 media stack.
9. `osdrv/opensource/kernel/` contains a Linux 4.9.37 patch and readmes; the availability and packaging of the complete kernel sources require further verification.

## First integration decision

Use the existing Linux MAPI client + LiteOS media-server path for the initial proof of concept. Do not assume a direct Linux VENC driver API exists for this board.

## Next source checks

- Inspect `middleware/sample/mw_sample` to identify a runnable media initialization example.
- Inspect `middleware/ndk/code` for the Hi3559V200 host/server sources and their build targets.
- Inspect `amp/a7_linux/mpp` and `amp/a7_liteos/mpp` for distributed headers/libraries and startup scripts.
- Compare candidate IMX458 configurations against factory boot evidence before choosing one.

## Validation status

Repository inspection only. No compilation, board access, or encoder measurements yet.
