#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include "../include/hi_mw_media_intf.h"
#include "comm_define.h"

#define MAX_VENC_START_CNT (5)

static pthread_mutex_t s_VEncLock = PTHREAD_MUTEX_INITIALIZER;

typedef struct tagVEncStartAttr_S
{
    HI_U32 u32StartCnt;
    HI_MW_OnVEncDataCB_FN apfnDataCB[MAX_VENC_START_CNT];
    HI_VOID* apPrivData[MAX_VENC_START_CNT];
} VEncStartAttr_S;

static VEncStartAttr_S s_astVEncStartAttrs[] =
{
    {0, {NULL,}, {NULL,}},
    {0, {NULL,}, {NULL,}},
    {0, {NULL,}, {NULL,}},
    {0, {NULL,}, {NULL,}},
    {0, {NULL,}, {NULL,}},
};

static HI_BOOL VEncHandleValid(HI_HANDLE handle);

/*
for comment retrace current not deal.
static HI_S32 VEncDataProc(HI_HANDLE hVEncHdl, HI_MAPI_VENC_DATA_S* pVStreamData, __attribute__((unused))HI_VOID* pPrivateData)
{
    HI_U32 i,j;
    HI_MW_MEDIA_VIDEO_S stVideoData;
    stVideoData.u32PackCount = pVStreamData->u32PackCount;
    stVideoData.u32Seq = pVStreamData->u32Seq;
    stVideoData.bEndOfStream = pVStreamData->bEndOfStream;
    for (i = 0; i < pVStreamData->u32PackCount; i++)
    {
        for(j = 0;j < HI_MW_PACK_NUM;j++)
        {
            stVideoData.astPack[i].u64PhyAddr[j] =(HI_U64) pVStreamData->astPack[i].au64PhyAddr[j];
            stVideoData.astPack[i].pu8Addr[j] = pVStreamData->astPack[i].apu8Addr[j];
            stVideoData.astPack[i].au32Len[j] = pVStreamData->astPack[i].au32Len[j];
        }
        stVideoData.astPack[i].u64PTS = pVStreamData->astPack[i].u64PTS;
        stVideoData.astPack[i].u32Offset = pVStreamData->astPack[i].u32Offset;
        memcpy(&stVideoData.astPack[i].stDataType, &pVStreamData->astPack[i].stDataType, sizeof(HI_MW_MEDIA_TYPE_S));
    }
    for (i = 0; i < s_astVEncStartAttrs[hVEncHdl].u32StartCnt; i++)
    {
        s_astVEncStartAttrs[hVEncHdl].apfnDataCB[i](hVEncHdl, &stVideoData, s_astVEncStartAttrs[hVEncHdl].apPrivData[i]);
    }
    return 0;
}
*/
static HI_S32 VEncDataProc(HI_HANDLE hVEncHdl, HI_MAPI_VENC_DATA_S* pVStreamData, __attribute__((unused))HI_VOID* pPrivateData)
{
    HI_U32 i, j;
    HI_MW_MEDIA_VIDEO_S stVideoData;
    if (!pVStreamData || !VEncHandleValid(hVEncHdl) ||
        pVStreamData->u32PackCount > HI_MW_MAX_FRAME_PACKCOUNT)
        return HI_FAILURE;
    memset(&stVideoData, 0, sizeof(stVideoData));
    stVideoData.u32PackCount = pVStreamData->u32PackCount;
    stVideoData.u32Seq = pVStreamData->u32Seq;
    stVideoData.bEndOfStream = pVStreamData->bEndOfStream;
    for ( i = 0 ; i < pVStreamData->u32PackCount; i++ )
    {
        HI_U64 u64PackPhyAddr = pVStreamData->astPack[i].au64PhyAddr[0];
        HI_U32 u32PackLen = pVStreamData->astPack[i].au32Len[0];
        HI_U32 u32PackOffset = pVStreamData->astPack[i].u32Offset;

        HI_U64 s_pDataVirtAddr = (HI_U64)pVStreamData->astPack[i].apu8Addr[1];
        HI_U64 s_u64DataPhyAddr = pVStreamData->astPack[i].au64PhyAddr[1];
        HI_U32 s_u32DataLen = pVStreamData->astPack[i].au32Len[1];
        HI_U8* pPackVirtAddr = pVStreamData->astPack[i].apu8Addr[0];
        if (u32PackOffset > u32PackLen ||
            (u32PackLen && !pPackVirtAddr) ||
            (s_u32DataLen && !s_pDataVirtAddr))
            return HI_FAILURE;

        if (s_u32DataLen && u64PackPhyAddr + u32PackLen >= s_u64DataPhyAddr + s_u32DataLen)
        {
            /* physical address retrace in offset segment */
            if (u64PackPhyAddr + u32PackOffset >= s_u64DataPhyAddr + s_u32DataLen)
            {
                stVideoData.astPack[i].pu8Addr[0] = (HI_U8*)(HI_UL)s_pDataVirtAddr +
                                                    ( (u64PackPhyAddr + u32PackOffset) - (s_u64DataPhyAddr + s_u32DataLen) );
                stVideoData.astPack[i].au32Len[0] = pVStreamData->astPack[i].au32Len[0] - pVStreamData->astPack[i].u32Offset;
                stVideoData.astPack[i].pu8Addr[1] = HI_NULL;
                stVideoData.astPack[i].au32Len[1] = 0;
                stVideoData.astPack[i].u32SegCnt = 1;
                stVideoData.astPack[i].u32Offset = 0;
            }
            else
            {
                /* physical address retrace in data segment */
                HI_U32 u32Left = (s_u64DataPhyAddr + s_u32DataLen) - u64PackPhyAddr;
                stVideoData.astPack[i].pu8Addr[0] = (HI_U8*)(pPackVirtAddr + u32PackOffset);
                stVideoData.astPack[i].au32Len[0] = u32Left - u32PackOffset;

                stVideoData.astPack[i].pu8Addr[1] = (HI_U8*)s_pDataVirtAddr;
                stVideoData.astPack[i].au32Len[1] = u32PackLen - u32Left;
                stVideoData.astPack[i].u32SegCnt = 2;
                stVideoData.astPack[i].u32Offset = 0;

            }
        }
        else
        {
            /* physical address retrace does not happen */
            stVideoData.astPack[i].pu8Addr[0] = pPackVirtAddr + u32PackOffset;
            stVideoData.astPack[i].au32Len[0] = u32PackLen - u32PackOffset;
            stVideoData.astPack[i].pu8Addr[1] = HI_NULL;
            stVideoData.astPack[i].au32Len[1] = 0;
            stVideoData.astPack[i].u32SegCnt = 1;
            stVideoData.astPack[i].u32Offset = 0;
        }
        stVideoData.astPack[i].u64PTS = pVStreamData->astPack[i].u64PTS;
        memcpy(&stVideoData.astPack[i].stDataType, &pVStreamData->astPack[i].stDataType, sizeof(HI_MW_MEDIA_TYPE_S));
    }
    /* Snapshot callbacks without holding a mutex across user code. */
    HI_MW_OnVEncDataCB_FN callbacks[MAX_VENC_START_CNT];
    HI_VOID *private_data[MAX_VENC_START_CNT];
    HI_U32 count;
    pthread_mutex_lock(&s_VEncLock);
    count = s_astVEncStartAttrs[hVEncHdl].u32StartCnt;
    for (i = 0; i < count; ++i)
    {
        callbacks[i] = s_astVEncStartAttrs[hVEncHdl].apfnDataCB[i];
        private_data[i] = s_astVEncStartAttrs[hVEncHdl].apPrivData[i];
    }
    pthread_mutex_unlock(&s_VEncLock);
    for (i = 0; i < count; ++i)
        if (callbacks[i]) callbacks[i](hVEncHdl, &stVideoData, private_data[i]);
    return HI_SUCCESS;

}

/* Handles are used as indices into the five-channel middleware table. */
static HI_BOOL VEncHandleValid(HI_HANDLE handle)
{
    return (handle >= 0 &&
            (HI_U32)handle < sizeof(s_astVEncStartAttrs) / sizeof(s_astVEncStartAttrs[0]));
}

HI_S32 HI_MW_VEnc_Start(HI_HANDLE hVEncHdl, HI_S32 s32FrameCnt,
                        HI_MW_OnVEncDataCB_FN pfnDataCB, HI_VOID* pPrivData)
{
    VEncStartAttr_S *attr;
    HI_U32 count;
    HI_S32 ret;

    if (!VEncHandleValid(hVEncHdl) || !pfnDataCB)
        return HI_FAILURE;

    pthread_mutex_lock(&s_VEncLock);
    attr = &s_astVEncStartAttrs[hVEncHdl];
    count = attr->u32StartCnt;
    if (count >= MAX_VENC_START_CNT || (s32FrameCnt > 0 && count != 0))
    {
        pthread_mutex_unlock(&s_VEncLock);
        return HI_FAILURE;
    }

    if (count == 0)
    {
        HI_MAPI_VENC_CALLBACK_S cb = {VEncDataProc, NULL};
        ret = HI_MAPI_VENC_RegisterCallback(hVEncHdl, &cb);
        if (ret != HI_SUCCESS)
        {
            pthread_mutex_unlock(&s_VEncLock);
            return ret;
        }
        ret = HI_MAPI_VENC_Start(hVEncHdl, s32FrameCnt);
        if (ret != HI_SUCCESS)
        {
            pthread_mutex_unlock(&s_VEncLock);
            return ret;
        }
    }

    attr->apfnDataCB[count] = pfnDataCB;
    attr->apPrivData[count] = pPrivData;
    attr->u32StartCnt = count + 1;
    pthread_mutex_unlock(&s_VEncLock);
    return HI_SUCCESS;
}

HI_S32 HI_MW_VEnc_Stop(HI_HANDLE hVEncHdl, HI_MW_OnVEncDataCB_FN pfnDataCB)
{
    VEncStartAttr_S *attr;
    HI_U32 i, count;
    HI_S32 ret;

    if (!VEncHandleValid(hVEncHdl) || !pfnDataCB)
        return HI_FAILURE;

    pthread_mutex_lock(&s_VEncLock);
    attr = &s_astVEncStartAttrs[hVEncHdl];
    count = attr->u32StartCnt;
    for (i = 0; i < count; ++i)
        if (attr->apfnDataCB[i] == pfnDataCB)
            break;

    if (i == count)
    {
        pthread_mutex_unlock(&s_VEncLock);
        return HI_FAILURE;
    }

    if (count == 1)
    {
        ret = HI_MAPI_VENC_Stop(hVEncHdl);
        if (ret != HI_SUCCESS)
        {
            pthread_mutex_unlock(&s_VEncLock);
            return ret;
        }
    }

    for (; i + 1 < count; ++i)
    {
        attr->apfnDataCB[i] = attr->apfnDataCB[i + 1];
        attr->apPrivData[i] = attr->apPrivData[i + 1];
    }
    attr->apfnDataCB[count - 1] = NULL;
    attr->apPrivData[count - 1] = NULL;
    attr->u32StartCnt = count - 1;
    pthread_mutex_unlock(&s_VEncLock);
    return HI_SUCCESS;
}

HI_S32 HI_MW_VEnc_GetStreamHeadInfo(HI_HANDLE hVEncHdl, HI_MW_VIDEO_HEAD_TYPE_E enType,
                                    HI_CHAR* pcHeadInfo, HI_U32* pu32HeadInfoLength)
{
    return HI_SUCCESS;
}
