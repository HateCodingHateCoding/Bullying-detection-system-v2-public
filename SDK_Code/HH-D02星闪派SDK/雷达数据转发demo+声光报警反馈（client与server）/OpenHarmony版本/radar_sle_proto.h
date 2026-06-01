#ifndef RADAR_SLE_PROTO_H
#define RADAR_SLE_PROTO_H

#include <stdint.h>

#define RADAR_SLE_MAGIC 0xD0D2

#define RADAR_SLE_TYPE_HIF_PKT    1
#define RADAR_SLE_TYPE_FRAME_END  2

#define RADAR_SLE_FLAG_FIRST  (1u << 0)
#define RADAR_SLE_FLAG_LAST   (1u << 1)

#pragma pack(push, 1)
typedef struct {
    uint16_t magic;      // 0xD0D2
    uint8_t  type;       // RADAR_SLE_TYPE_*
    uint8_t  flags;      // FIRST/LAST
    uint16_t seq;        // 包序号（每个HIF包+1 或 每帧+1 都行，下面按“每个HIF包+1”）
    uint16_t total_len;  // 原始数据总长度（一个HIF包：6 + payload + 4）
    uint16_t offset;     // 当前分片在 total_len 内的偏移
    uint16_t chunk_len;  // 当前分片有效数据长度
} radar_sle_hdr_t;
#pragma pack(pop)

#define RADAR_SLE_HDR_LEN ((uint16_t)sizeof(radar_sle_hdr_t))

#endif