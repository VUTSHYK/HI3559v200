#include "venc_probe.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    FPV_VENC_STATS s = {0};
    HI_MW_MEDIA_VIDEO_S v = {0};
    HI_U8 a[4] = {0}, b[3] = {0};

    v.u32PackCount = 1;
    v.u32Seq = 7;
    v.astPack[0].u32SegCnt = 2;
    v.astPack[0].pu8Addr[0] = a;
    v.astPack[0].au32Len[0] = sizeof(a);
    v.astPack[0].pu8Addr[1] = b;
    v.astPack[0].au32Len[1] = sizeof(b);
    v.astPack[0].u64PTS = 1000;
    assert(FPV_VencProbeCallback(0, &v, &s) == HI_SUCCESS);
    assert(s.callbacks == 1 && s.packs == 1 && s.bytes == 7);
    assert(s.first_seq == 7 && s.last_pts == 1000);

    v.u32Seq = 8;
    v.astPack[0].u64PTS = 2000;
    assert(FPV_VencProbeCallback(0, &v, &s) == HI_SUCCESS);
    assert(s.callbacks == 2 && s.bytes == 14 && s.last_pts == 2000);

    v.astPack[0].pu8Addr[1] = NULL;
    assert(FPV_VencProbeCallback(0, &v, &s) == HI_FAILURE);
    assert(s.invalid == 1 && s.callbacks == 2);

    v.astPack[0].pu8Addr[1] = b;
    v.astPack[0].u32SegCnt = 3;
    assert(FPV_VencProbeCallback(0, &v, &s) == HI_FAILURE);
    assert(s.invalid == 2);

    v.astPack[0].u32SegCnt = 1;
    v.u32PackCount = HI_MW_MAX_FRAME_PACKCOUNT + 1;
    assert(FPV_VencProbeCallback(0, &v, &s) == HI_FAILURE);
    assert(s.invalid == 3);
    FPV_VencProbePrint(&s);
    puts("probe tests: PASS");
    return 0;
}
