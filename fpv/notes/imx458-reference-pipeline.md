# IMX458 reference media pipeline (Hi3559V200)

## Verified source paths

The reference action-camera application provides actual IMX458 media profiles:

- `reference/actioncam/modules/param/inicfg/hi3559v200/imx458_256M/config_product_mediamode_1080p90_rec.ini`
- `reference/actioncam/modules/param/inicfg/hi3559v200/imx458_256M/config_product_mediamode_4k2k30_rec.ini`
- `reference/actioncam/modules/param/inicfg/hi3559v200/imx458_256M/config_product_media_common.ini`

These are SDK **reference-board** profiles, not measurements from the target FPV camera.

| Property | 1080p90 reference | 4K30 reference |
| --- | --- | --- |
| Sensor/VCAP | 1920x1080 @ 90 | 3840x2160 @ 30 |
| WDR | none | none |
| VI/VPSS | VI_ONLINE_VPSS_OFFLINE | VI_ONLINE_VPSS_OFFLINE |
| VPSS 0 output 0 | 1920x1080 @ 90 | 3840x2160 @ 30 |
| VPSS 0 output 1 | 1024x576 @ 30 | 1024x576 @ 30 |
| VENC 0 | 1920x1080 @ 90 | 3840x2160 @ 30 |
| VENC 0 GOP | 30 | 30 |
| VENC 0 H264 target bitrate | 43008 kbps | 55296 kbps |
| VENC 0 H265 target bitrate | 35840 kbps | 46080 kbps |
| VENC 0 buffer size | 12441600 bytes | 12441600 bytes |
| VENC 1 | 1024x576 @ 30 | 1024x576 @ 30 |

Common media profile sets `[venc.video.main] payload_type="1"`
(H.265) and `[venc.video.sub] payload_type="0"` (H.264).
The media-mode files specify bitrate for **both** codecs but do not
independently select H.264. A low-latency H.264 experiment must explicitly
configure the main codec and verify the actual output bitstream.

## Suggested controlled experiment

1. Establish exact board model and memory split; do not flash the reference config.
2. Identify whether the stock LiteOS media service is already running and
   owns the encoder. Avoid a second concurrent owner.
3. Confirm a known-good baseline stream and log actual codec, FPS, and PTS.
4. For a separate IMX458 test image, derive a single-main-channel H.264
   1080p90 profile from the *reference* settings above, with optional
   secondary channels disabled only after verifying the binding graph.
5. Capture packet sizes, timestamps, keyframe cadence, and end-to-end
   latency; tune GOP/bitrate only after a stable baseline.

## Important limitations

- Sensor FPS is not guaranteed delivered FPS or end-to-end FPV latency.
- These SDK files do not establish target camera MIPI lanes, clocks,
  GPIO reset wiring, RAM capacity or MMZ allocation.
- No hardware tests or target cross-build have yet been performed.
- Do not modify `middleware/sample/mw_sample` to assume these values
  match the target board.
