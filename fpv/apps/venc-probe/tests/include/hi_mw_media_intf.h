/* Minimal ABI-shaped test double. Never use for target builds. */
#ifndef FPV_TEST_HI_MW_MEDIA_INTF_H
#define FPV_TEST_HI_MW_MEDIA_INTF_H
#include <stdint.h>
typedef uint8_t HI_U8;
typedef uint32_t HI_U32;
typedef uint64_t HI_U64;
typedef int HI_S32;
typedef int HI_BOOL;
typedef int HI_HANDLE;
typedef void HI_VOID;
#define HI_TRUE 1
#define HI_FALSE 0
#define HI_SUCCESS 0
#define HI_FAILURE (-1)
#define HI_MW_PACK_NUM 2
#define HI_MW_MAX_FRAME_PACKCOUNT 12
typedef struct {
    HI_U8 *pu8Addr[HI_MW_PACK_NUM];
    HI_U32 au32Len[HI_MW_PACK_NUM];
    HI_U64 u64PTS;
    HI_U32 u32SegCnt;
} HI_MW_MEDIA_PACK_S;
typedef struct {
    HI_MW_MEDIA_PACK_S astPack[HI_MW_MAX_FRAME_PACKCOUNT];
    HI_U32 u32PackCount;
    HI_U32 u32Seq;
    HI_BOOL bEndOfStream;
} HI_MW_MEDIA_VIDEO_S;
#endif
