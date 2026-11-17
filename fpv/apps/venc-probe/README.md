# VENC callback probe (first code milestone)

This is a **callback module**, not a standalone camera application. It uses the
actual SDK `HI_MW_MEDIA_VIDEO_S` and `HI_MW_OnVEncDataCB_FN` interface.

## Integrate into an SDK host-side sample

1. Build the middleware media adapter and a compatible sample for the exact board.
2. Include `venc_probe.h` in that sample and add `venc_probe.c` to its sources.
3. Allocate a `FPV_VENC_STATS stats = {0};` that remains valid while callbacks run.
4. After `HI_MW_Media_Init` and VENC channel initialization, subscribe with:
   `HI_MW_VEnc_Start(venc_handle, -1, FPV_VencProbeCallback, &stats)`.
5. Stop with `HI_MW_VEnc_Stop(venc_handle, FPV_VencProbeCallback)`
   **before** destroying `stats` or tearing down media. Print stats after stopping.

The encoder handle must be a configured **VENC channel**, not the sensor handle.
Do not start a second independent media stack if the stock camera service already
owns the encoder. First confirm whether attaching another callback is supported.

## What this measures

Callback invocations, packet count, encoded bytes, sequence/PTS endpoints and
invalid packet metadata. Sequence number continuity and PTS units are **not**
assumed. FPS is not computed yet because SDK PTS timebase and callback-to-frame
semantics require validation. No video buffers are retained or sent over network.

## Limits and safety

- No binary, compilation, or hardware run has been verified.
- No sensor initialization or flash modifications are performed here.
- The callback checks segment count and non-null pointers, but it cannot validate
  that vendor-supplied virtual addresses are mapped.
- Packet lengths may represent data spanning wrapped buffers; preserve all
  segments. No zero-copy forwarding until buffer ownership is understood.
- `mw_sample` currently lacks an IMX458 profile. Do not deploy it unchanged.
- The current middleware implementation has additional error-handling concerns
  around VENC start/stop; review before using this in a long-running service.

## SDK-header cross-compile smoke test

From `fpv/apps/venc-probe`, run:

```sh
make CC=arm-himix100-linux-gcc
```

This compiles `venc_probe.o` using the **actual** SDK headers rather than
the test doubles in `tests/include`. It does not link a target executable.
The cross compiler must be installed and available on `PATH`. This target
has not yet been executed or validated against the hardware.

For the isolated native unit tests:

```sh
make -C tests test
```

## Why the stock mw_sample is not yet an IMX458 executable

`middleware/cfg.mak` identifies `CFG_CHIP_TYPE=hi3559v200`, but
`middleware/sample/mw_sample/Makefile` has no `hi3559v200` branch
and falls back to `SENSOR_IMX477`. Its `venc_wrapper.c` likewise has
no `SENSOR_IMX458` initialization block. An IMX458 build needs validated
sensor dimensions, FPS, MIPI lane mapping, VPROC ports and VENC settings
from the **actual camera board**. Selecting an SDK config filename alone
does not establish board compatibility. We deliberately have not copied
IMX477 values or changed the vendor sample until those values are checked.
