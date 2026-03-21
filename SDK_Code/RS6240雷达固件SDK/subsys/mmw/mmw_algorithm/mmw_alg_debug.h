/*
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */
#ifndef __MMW_DEBUG_H__
#define __MMW_DEBUG_H__

/* Include Files */
#include "config.h"
#include "mmw_type.h"
#include "mdsp_driver.h"
#include "hif.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AHB_TIMER_FREQ_MHZ     112
#define CORET_TO_US(time)       ((time)/AHB_TIMER_FREQ_MHZ)
#define US_TO_CORET(time)       ((time)*AHB_TIMER_FREQ_MHZ)


#if (POINTCLOUD_SHOW_TIME_INFO)

typedef struct _time_dbg_info_t {
	char    *label;
	uint32_t cnt;
	uint32_t min;
	uint32_t max;
	uint64_t sum;
} TIME_DBG_INFO;

typedef struct _points_proc_time_t {
	TIME_DBG_INFO motion_point;
	TIME_DBG_INFO micro_point;
	TIME_DBG_INFO motion_convert;
	TIME_DBG_INFO report;
	TIME_DBG_INFO report_block;

	TIME_DBG_INFO micro_cube_store;
	TIME_DBG_INFO micro_cube_read;
	TIME_DBG_INFO micro_dop;
	TIME_DBG_INFO micro_cfar;
} DBG_POINTS_PROC_TIME;
extern DBG_POINTS_PROC_TIME g_time_dbg_info;

#define SET_TIME_START(t)		((t) = csi_coret_get_value())
#define ADD_TIME_ESCAPE(v, t)   ((v) += time_diff(csi_coret_get_value(), (t)))
#define SET_TIME_ESCAPE(v, t)	((v) = time_diff(csi_coret_get_value(), (t)))
#define CALC_TIME_ESCAPE(t)     (time_diff(csi_coret_get_value(), (t)))

#define UPDATE_TIME_DBG_INFO(t, name, time_info)  do { \
	if (time_info.label != (name) && time_info.cnt == 0) { \
		time_info.min = (t); \
		time_info.max = (t); \
		time_info.label = (name); \
	} else { \
		if ((t) > time_info.max) { time_info.max = (t); } \
		else if ((t) < time_info.min) { time_info.min = (t); } \
	} \
	time_info.sum += (t); \
	time_info.cnt++; \
} while (0)

#else

#define SET_TIME_START(t)        (void)(t)
#define ADD_TIME_ESCAPE(v, t)
#define SET_TIME_ESCAPE(v, t)
#define CALC_TIME_ESCAPE(t)      0
#define UPDATE_TIME_DBG_INFO(t, lable, time_info)  (void)(t)

#endif

/* PSIC DEBUG PROTOCOL */

/* 
 * @brief Data format: byte/short/long/floating
 * */
typedef enum {
    PSIC_DBG_PROTO_DATA_FORMAT_BYTE = 0,
    PSIC_DBG_PROTO_DATA_FORMAT_SHORT = 1,
    PSIC_DBG_PROTO_DATA_FORMAT_LONG = 2,
    PSIC_DBG_PROTO_DATA_FORMAT_FLOATING = 3
}PsicDbgProtoPayloadDataFormat_e;

/* 
 * @brief Data format: signed/unsigned for byte/short/long, f32/double for floating
 * */
typedef enum {
    PSIC_DBG_PROTO_DATA_UNSIGNED = 0,
    PSIC_DBG_PROTO_DATA_SIGNED
}MotionSensorPayloadDataSign_e;
typedef enum {
    PSIC_DBG_PROTO_DATA_F32 = 0,
    PSIC_DBG_PROTO_DATA_F64
}MotionSensorPayloadFloatingType_e;

typedef struct {
    uint8_t q_format;
    uint8_t align_mode;
    uint8_t dim;
    uint16_t len;
    PsicDbgProtoPayloadDataFormat_e payload_data_format;
    MotionSensorPayloadDataSign_e payload_data_sign;
} PsicDegProtoHeader_t;

/**
 * @brief Upload debug data via HIF protocol
 * @param ptr_data data buffer to be upload, can be varaies pointer types;
 * @param ptr_channel_name name to display in RadarDebugTool;
 * @param dim dimension for a single upload object
 * @param len length length of objects number, len * dim is the total data should be upload
 * @param data_format byte/short/long/floating, ref to PsicDbgProtoPayloadDataFormat_e
 * @param signed_flag refer to MotionSensorPayloadDataSign_e & MotionSensorPayloadFloatingType_e
 * @param align_mode only 0&1 are support. Some point coordinate (x0, y0), (x1, y1) will display in RadarDebugTool: 
 * 		if align_mode=0, ptr_data must store [x0,x1,x2,...y0,y1,y2...], 
 * 		if align_mode=1,ptr_data must store [x0,y0,x1,y1,...];
 * @param q_value final result is data in ptr_data / power(2, q_value), this value is usually set to 0.
 * */ 
extern void mmw_psic_debug_proto_report(void *ptr_data, char *ptr_channel_name, uint8_t dim, uint16_t len, PsicDbgProtoPayloadDataFormat_e data_format, uint8_t signed_flag, uint8_t align_mode, uint8_t q_value);
/**
 * @brief Upload debug data via HIF protocol
 * @param ptr_data data buffer to be upload, can be varaies pointer types;
 * @param ptr_channel_name name to display in RadarDebugTool;
 * @param dim dimension for a single upload object
 * @param len length length of objects number, len * dim is the total data should be upload
 * @param data_format byte/short/long/floating, ref to PsicDbgProtoPayloadDataFormat_e
 * @param signed_flag refer to MotionSensorPayloadDataSign_e & MotionSensorPayloadFloatingType_e
 * @param align_mode only 0&1 are support. Some point coordinate (x0, y0), (x1, y1) will display in RadarDebugTool: 
 * 		if align_mode=0, ptr_data must store [x0,x1,x2,...y0,y1,y2...], 
 * 		if align_mode=1,ptr_data must store [x0,y0,x1,y1,...];
 * @param sentCb,the callback function after a package of data transmission is completed
 * @param q_value final result is data in ptr_data / power(2, q_value), this value is usually set to 0.
 * @return return 0 indicates that the HIF task has been added successfully,and return other values indicates failure.
 * */ 
extern int8_t mmw_psic_debug_proto_report_async(void *ptr_data, char *ptr_channel_name, uint8_t dim, uint16_t len, PsicDbgProtoPayloadDataFormat_e data_format, uint8_t signed_flag, uint8_t align_mode, uint8_t q_value, HIF_MsgTrans_Callback sentCb);

void show_complex_vec(const char *prefix, complex16_mdsp *data, uint32_t print_num, uint32_t step);
void show_u32_vec(const char *prefix, uint32_t *data, uint32_t print_num, uint32_t step);

/* the callback function for releasing all buffer in non-blocking mode */
extern void mmw_psic_debug_protocol_free_all_cb(hif_queue_item_t *hif_queue_item, int8_t status);
/* the callback function for releasing name and header buffer in non-blocking mode */
extern void mmw_psic_debug_protocol_free_header_channel_name_cb(hif_queue_item_t *hif_queue_item, int8_t status);
#ifdef __cplusplus
}
#endif

#endif /* __MMW_DEBUG_H__ */

