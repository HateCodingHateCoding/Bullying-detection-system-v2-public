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
    uint16_t magic;
    uint8_t type;
    uint8_t flags;
    uint16_t seq;
    uint16_t total_len;
    uint16_t offset;
    uint16_t chunk_len;
} radar_sle_hdr_t;
#pragma pack(pop)

#define RADAR_SLE_HDR_LEN ((uint16_t)sizeof(radar_sle_hdr_t))

#endif
