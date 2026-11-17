# LiteOS media link audit — confirmed build inputs

Date: 2026-11-17. Static repository inspection only.

## Final link

`reference/actioncam/modules/init/amp/liteos/Makefile`:
- Declares `VSS_LIB` with `-lmpi`, `-l$(CFG_CHIP_TYPE)_isp`,
  `_vi`, `_vpss`, `_vedu`, `_venc`, `_h264e`, `_h265e`,
  `_chnl`, `_rc` and other dependencies.
- For `CONFIG_HI3559V200=y`, links
  `libipcmsg_a7_liteos.a` and `libdatafifo_a7_liteos.a`.
- For `CFG_SENSOR_TYPE0=imx458`, links `-lsns_imx458`
  and `-l$(CFG_CHIP_TYPE)_mapi_cfg_imx458`.
- Output `media_app`, binary `media_app.bin`, and linker map
  `media_app.map`.
- Final link at line ~262 uses `$(LD)`, `$(VSS_LDFLAGS)`,
  `$(LINK_MAP)`, `$(OBJS)`, `--start-group $(VSS_LIB)
  --end-group`.
- Build copies `$(MPP_REL_LITEOS_LIB_PATH)/*.a` and
  other archives into the final LiteOS library directory.

## Source availability

The tracked Git tree (8057 entries, not truncated) has no
`libmpi.a`, `libhi3559v200_venc.a`,
`libhi3559v200_vedu.a`, or H264E/H265E archives.
It also has no obvious VENC/VEDU/H264E/H265E driver source
directories; this observation concerns tracked repository files only.

There **is** partial ISP driver source:
`amp/a7_liteos/mpp/component/isp/kernel/mkp/src/Makefile`
lists `isp.c`, `isp_st.c`, `isp_drv_vreg.c`,
`isp_list.c`. Do not extrapolate this to VENC availability.

## Assessment

The vendor's actioncam image expects separate native MPI,
VENC, VEDU and codec libraries. The relevant archives are not
tracked in this fork; the next task is to locate the original
vendor SDK package or extracted build output and inspect its
symbol tables and ELF architecture. A linker recipe is not
equivalent to a working Linux driver.

## Next evidence needed

- Locate `media_app.map` or `media_app` from an actual
  build to identify which archive/object defines
  `HI_MPI_VENC_CreateChn`, `venc_mod_init`,
  `vedu_mod_init`, `h264e_mod_init`.
- Search SDK archive and extracted camera images for
  `libmpi.a`, `libhi3559v200_venc.a`,
  `libhi3559v200_vedu.a`, codec archives.
- Analyze using `file`, `readelf -h`,
  `arm-none-eabi-nm -A` or the appropriate SDK toolchain.
- Compare Linux build libraries separately; do not infer
  Linux ABI compatibility from LiteOS static archives.
