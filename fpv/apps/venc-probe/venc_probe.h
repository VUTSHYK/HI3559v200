#ifndef FPV_VENC_PROBE_H
#define FPV_VENC_PROBE_H

#include "hi_mw_media_intf.h"

typedef struct {
    HI_U64 callbacks;
    HI_U64 packs;
    HI_U64 bytes;
    HI_U64 invalid;
    HI_U64 first_pts;
    HI_U64 last_pts;
    HI_U32 first_seq;
    HI_U32 last_seq;
    HI_BOOL seen;
} FPV_VENC_STATS;

HI_S32 FPV_VencProbeCallback(HI_HANDLE handle, HI_MW_MEDIA_VIDEO_S *video, HI_VOID *userdata);
void FPV_VencProbePrint(const FPV_VENC_STATS *stats);

#endif
