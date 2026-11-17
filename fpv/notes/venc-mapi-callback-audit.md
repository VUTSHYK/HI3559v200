# Hi3559V200 MAPI VENC callback audit

Source: `middleware/media_adpt/hi3559v200/venc_ops.c` on
`feature/openipc-fpv`. This is a static code review, not a runtime test.

## Data path

`HI_MAPI_VENC_RegisterCallback` registers the local `VEncDataProc`.
It converts `HI_MAPI_VENC_DATA_S` packets into
`HI_MW_MEDIA_VIDEO_S`, splitting a wrapped physical buffer into up to
two virtual-address segments. It dispatches the resulting stack-allocated
structure to each registered `HI_MW_OnVEncDataCB_FN` synchronously.

The FPV callback must not retain any pointer from the callback. It
currently counts metadata and bytes without retaining the buffers.

## Issues confirmed by inspection

1. `HI_MW_VEnc_Start` locks `s_VEncLock` and returns on the
   callback-count limit without unlocking it: subsequent calls may
   deadlock.
2. `HI_MW_VEnc_Stop` searches up to `u32StartCnt` but checks
   `i == MAX_VENC_START_CNT` rather than `i == u32StartCnt`.
   Missing callbacks can incorrectly fall through into removal.
   That failure path also returns without unlocking.
3. Stop's shift loop uses `j < u32StartCnt` and reads
   `[j+1]` on its final iteration; at capacity this may be
   out of bounds. It should shift only until `j+1 < u32StartCnt`
   and clear the vacated last slot.
4. Start ignores return values from `HI_MAPI_VENC_RegisterCallback`
   and `HI_MAPI_VENC_Start`, reporting success even if the
   media server rejects the operation.
5. The MAPI-to-MW adapter loops over `pVStreamData->u32PackCount`
   without checking that it fits the destination
   `HI_MW_MAX_FRAME_PACKCOUNT` array.
6. Adapter packet math subtracts `u32Offset` from lengths and
   combines physical addresses without validating offset/ranges.
   Invalid metadata may underflow or produce bad virtual pointers.
7. The dispatch loop accesses `s_astVEncStartAttrs` without holding
   the same mutex as start/stop; callback removal and dispatch need
   a clearly documented synchronization strategy. Do not simply
   hold the mutex while invoking arbitrary callbacks: that could
   deadlock re-entrant clients.

## Consequences for FPV

This wrapper is **not yet safe to treat as a reliable streaming API**.
Especially under repeated start/stop or malformed buffers, errors could
be confused with codec/latency problems. Before using it for a long
running Waybeam sender, isolate and fix these behaviors and test the
adapter with mocked MAPI packets.

## Next implementation steps

- Write isolated tests for start/stop registration, callback absence,
  capacity, MAPI failures and wraparound metadata.
- Apply a small, reviewable patch to the SDK wrapper, preserving
  the actual callback ABI and existing semantics.
- Build with the vendor ARM toolchain; then run a bounded, single-owner
  hardware test with stock firmware preserved.
- Only after a stable baseline measure 720p120/1080p90 latency and
  investigate VPSS buffering and any supported low-delay modes.

No vendor source files were modified in this audit.
