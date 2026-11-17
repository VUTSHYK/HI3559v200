# VENC MPI implementation trace — concrete source evidence

Date: 2026-11-17. Repository tree scanned using GitHub Git Trees
API with `recursive=1`: 8057 entries, `truncated=false`.

## Binary inventory (tracked repository files only)

No tracked paths matching `libmpi*.a`, `libmpi*.so`,
`*venc*.ko` or `*venc*.a` appeared in the full tree.
**This does not exclude untracked SDK archives, generated outputs,
Git LFS payloads, or files from the camera dump.**

## Confirmed native MPI callsites

`middleware/ndk/code/mediaserver/venc/mapi_venc.c`:
- includes `mpi_venc.h` and `mpi_sys.h`;
- `MAPI_VENC_Init` calls `HI_MPI_VENC_GetModParam` /
  `HI_MPI_VENC_SetModParam`;
- `HI_MAPI_VENC_Init` calls `HI_MPI_VENC_CreateChn`;
- channel attribute control calls `HI_MPI_VENC_GetChnAttr` /
  `HI_MPI_VENC_SetChnAttr`.

`middleware/ndk/code/mediaserver/venc/arch/hi3559v200/mapi_venc_adapt.c`:
- calls `HI_MPI_VENC_GetH265Trans` and
  `HI_MPI_VENC_SetH265Trans` for car-scene H.265;
- maps MAPI rate-control attributes to native VENC attributes.

`middleware/ndk/code/mediaserver/venc/os/dual/mapi_venc_os.c`:
- includes `hi_datafifo.h`;
- uses `HI_MPI_VENC_GetStreamBufInfo` to discover buffer layout;
- uses `HI_MPI_VENC_GetStream` to receive encoded packets;
- uses `HI_DATAFIFO_Write` and `DATAFIFO_CMD_WRITE_DONE` to
  forward stream metadata across the AMP boundary;
- calls `HI_MPI_VENC_ReleaseStream` on the producer side;
- invokes `HI_MPI_VENC_StopRecvFrame` when stopping.

`middleware/ndk/code/mediaclient/venc/mapi_venc_client.c`
is the Linux-side MAPI client counterpart.

## Conclusion

We have found **the direct caller of the native MPI VENC API** and
the stream handoff code. We have **not** found the actual
`HI_MPI_VENC_*` implementation or linked vendor library in tracked
repository files. The implementation may reside in a binary SDK
artifact absent from Git or in LiteOS media drivers.

## Next actions

1. Inspect media server Makefiles/link flags for actual `-lmpi`
   or static objects and the selected LiteOS build target.
2. Inspect `amp/a7_liteos/mpp/init/sdk_init.c` module init
   sequence and `amp/a7_liteos/mpp/include/mpi_venc.h`.
3. Search the separate original SDK archive and camera dumps
   for MPI binaries; use `readelf`, `nm`, `strings`, `file`.
4. Map `HI_MPI_VENC_GetStream` → DATAFIFO → Linux MAPI callback
   and measure whether copying or buffering contributes to latency.
5. Keep all experiments read-only until a reversible RAM boot exists.

This is source tracing, not evidence that Linux-only VENC works.
