# FPV mode strategy: OpenIPC comparison, Hi3559V200 + IMX458

## Why the recording presets are not the final FPV design

OpenIPC documents that end-to-end latency is dominated by the whole
sensor → ISP → VI/VPSS → VENC → transport → decoder → display path.
Higher actual frame rate, low-delay processing, bounded exposure, and
receiver buffering all matter. This is a **design comparison**, not a
claim that OpenIPC's measurements apply to Hi3559V200.

Public sources (reviewed 2026-11-17):
- https://github.com/OpenIPC/wiki/blob/master/en/fpv.md
- https://github.com/OpenIPC/openhisilicon/blob/main/docs/sensor-pipeline-latency-metering.md
- https://github.com/OpenIPC/wiki/blob/master/en/majestic-encoder-tuning.md
- https://github.com/OpenIPC/configurator/blob/master/majestic.yaml

## Actual SDK profiles for IMX458 256M reference board

All are under
`reference/actioncam/modules/param/inicfg/hi3559v200/imx458_256M/`:

| Profile filename suffix | Capture/output | SDK status |
| --- | --- | --- |
| `720p120_rec.ini` | 1280x720 @120 | Present |
| `1080p90_rec.ini` | 1920x1080 @90 | Present |
| `1080p60_rec.ini` | 1920x1080 @60 | Present |
| `4k2k30_rec.ini` | 3840x2160 @30 | Present |

The 720p120 reference uses `VI_ONLINE_VPSS_OFFLINE`, VCAP
and primary VPSS/VENC at 120fps, a secondary VPSS output at 30fps,
and `dis_support=0`. The IMX458 driver additionally lists a
1280x720 nominal 128.8fps mode, but the reference pipeline selects
120fps. Do not conflate nominal sensor maximum with configured rate.

## Experiments, in priority order

1. **FPV-720p120:** first latency-oriented experiment. Verify
   sensor/encoder both deliver 120fps, no unintentional downsampling.
2. **FPV-1080p90:** more detail, compare delivered FPS and latency.
3. **FPV-1080p60:** compatibility fallback.
4. **FPV-1524p60:** experimental mode; sensor driver lists
   2716x1524@59.94 but a matching reference actioncam profile has
   **not** been verified. Requires new, validated pipeline config.
5. **4K30:** quality benchmark, not the primary low-latency target.

## Settings to investigate, not blindly copy

- Prefer one active main stream while benchmarking; inspect
  actual binding graph before disabling auxiliary streams.
- H.264 and H.265 must be benchmarked against receiver support and
  packet loss. Common SDK main profile currently selects H.265.
- GOP and bitrate must account for wireless loss/recovery, not just
  compression efficiency.
- Exposure upper bound: approximately 8.33ms at 120fps, 11.11ms at
  90fps, subject to actual AE implementation and available light.
- Investigate Hi3559V200-specific ISP/VPSS low-delay options, if any;
  do not assume an OpenIPC Majestic setting maps directly to this SDK.
- Compare sensor FPS, encoded FPS, timestamps, frame drops, packet
  sizes, radio latency and receiver/display buffering separately.

## Safety and validation gates

No hardware tested. No target ARM build confirmed. Do not overwrite
the factory firmware or run two competing media-service owners.
Confirm board memory layout and IMX458 wiring before deploying.
