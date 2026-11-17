# VENC middleware patch validation status

Patch: `67b8af13a3f174ad9e86547d04018661620c5123`
(`middleware/media_adpt/hi3559v200/venc_ops.c`).

## Implemented

- Validate channel index and non-NULL callback.
- Unlock mutex on rejected starts/stops.
- Check MAPI register/start/stop return values.
- Correct callback removal and clear vacated slot.
- Reject oversized MAPI packet count and invalid offset.
- Snapshot callback list under mutex; dispatch without holding it.

## Remaining risks / do not deploy yet

1. **Callback lifetime:** a snapshot may invoke a callback after another
   thread unregisters it. Caller-owned `pPrivData` must remain valid
   until in-flight dispatch is drained. Add active-dispatch tracking
   and explicit shutdown semantics before production use.
2. **Reentrancy:** `HI_MAPI_VENC_Start/Stop` still execute under the
   mutex. Verify whether MAPI can invoke callbacks synchronously;
   if so, refactor lifecycle to avoid deadlock.
3. **Buffer validation:** address addition overflow, wrapped-buffer
   bounds, and physical-to-virtual assumptions require targeted
   malformed-packet tests. This patch is not a full packet-parser proof.
4. **Callback registration rollback:** when RegisterCallback succeeds
   but Start fails, SDK callback registration may remain. Verify
   whether the API provides unregister and implement rollback.
5. **Compiler and ABI:** no ARM cross-compile or device test has been
   performed. Validate against the actual HiSilicon headers/toolchain.
6. **Finite frame count:** multiple subscribers are currently rejected
   when `s32FrameCnt > 0` and the channel is already active; confirm
   required semantics before generalizing this API.

## Recommended gate

Do not ship this as a stable Waybeam transport until native mocked
MAPI tests, ARM build, and a bounded on-device stress test pass.
