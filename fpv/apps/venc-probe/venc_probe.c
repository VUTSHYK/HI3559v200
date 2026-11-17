/* Hi3559V200 MAPI VENC callback probe.
 * Compile against the SDK middleware/media_adpt/include headers.
 * The caller owns media initialization and HI_MW_VEnc_Start/Stop.
 * This callback never retains pointers into SDK-managed packet buffers.
 */
#include "venc_probe.h"
#include <inttypes.h>
#include <stdio.h>

HI_S32 FPV_VencProbeCallback(HI_HANDLE handle, HI_MW_MEDIA_VIDEO_S *video, HI_VOID *userdata)
{
    FPV_VENC_STATS *s = (FPV_VENC_STATS *)userdata;
    HI_U32 i, j;
    HI_U64 bytes = 0;
    (void)handle;

    if (!s || !video || video->u32PackCount > HI_MW_MAX_FRAME_PACKCOUNT) {
        if (s) ++s->invalid;
        return HI_FAILURE;
    }

    for (i = 0; i < video->u32PackCount; ++i) {
        const HI_MW_MEDIA_PACK_S *p = &video->astPack[i];
        if (p->u32SegCnt == 0 || p->u32SegCnt > HI_MW_PACK_NUM) {
            ++s->invalid;
            return HI_FAILURE;
        }
        for (j = 0; j < p->u32SegCnt; ++j) {
            if (p->au32Len[j] && !p->pu8Addr[j]) {
                ++s->invalid;
                return HI_FAILURE;
            }
            bytes += p->au32Len[j];
        }
    }

    if (!s->seen) {
        s->first_seq = video->u32Seq;
        s->first_pts = video->u32PackCount ? video->astPack[0].u64PTS : 0;
        s->seen = HI_TRUE;
    }
    s->last_seq = video->u32Seq;
    if (video->u32PackCount)
        s->last_pts = video->astPack[0].u64PTS;
    s->bytes += bytes;
    s->packs += video->u32PackCount;
    ++s->callbacks;
    return HI_SUCCESS;
}

void FPV_VencProbePrint(const FPV_VENC_STATS *s)
{
    if (!s) return;
    printf("VENC callbacks=%" PRIu64 " packs=%" PRIu64
           " bytes=%" PRIu64 " invalid=%" PRIu64
           " seq=%u..%u pts=%" PRIu64 "..%" PRIu64 "\n",
           (uint64_t)s->callbacks, (uint64_t)s->packs,
           (uint64_t)s->bytes, (uint64_t)s->invalid,
           (unsigned)s->first_seq, (unsigned)s->last_seq,
           (uint64_t)s->first_pts, (uint64_t)s->last_pts);
}
