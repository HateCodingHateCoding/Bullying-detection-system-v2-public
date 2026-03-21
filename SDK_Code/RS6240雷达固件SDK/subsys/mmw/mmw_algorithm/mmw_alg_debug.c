/*
 **************************************************************************************************
 *          Copyright (c) 2022 Possumic Technology. all rights reserved.
 **************************************************************************************************
 */
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "mmw_ctrl.h"
#include "mmw_alg_debug.h"
#include "hif.h"
#include "mmw_alg_pointcloud.h"

#if(POINTCLOUD_SHOW_TIME_INFO)
DBG_POINTS_PROC_TIME g_time_dbg_info;
#endif

void show_u32_vec(const char *prefix, uint32_t *data, uint32_t print_num, uint32_t step)
{
	if (prefix) {
		printk("%s addr=%p num=%d\n", prefix, data, print_num);
	}
	for (int i = 0; i < print_num * step; i += step) {
		printk("%08x\t", data[i]);
	}
	printk("\n");
}

void show_complex_vec(const char *prefix, complex16_mdsp *data, uint32_t print_num, uint32_t step)
{
	if (prefix) {
		printk("%s addr=%p num=%d\n", prefix, data, print_num);
	}
	for (int i = 0; i < print_num * step; i += step) {
		printk("%d\t", data[i].real);
	}
	printk("\n");
	for (int i = 0; i < print_num * step; i += step) {
		printk("%d\t", data[i].imag);
	}
	printk("\n");
}

void mmw_psic_debug_protocol_free_all_cb(hif_queue_item_t *hif_queue_item, int8_t status)
{
	payload_node_t *payload_head = hif_queue_item->payload;
	while (1)	// when the data report is completed,three buffer need to free(header, ptr_channel_name and ptr_data)
	{
		mmw_process_mem_free((void**)&payload_head->payload);
		if (payload_head->next == NULL)
		{
			break;
		}
		payload_head = payload_head->next;
	}
}

void mmw_psic_debug_protocol_free_header_channel_name_cb(hif_queue_item_t *hif_queue_item, int8_t status)
{
	payload_node_t *payload_head = hif_queue_item->payload;
	mmw_process_mem_free((void**)&payload_head->payload);	//first is header buffer
	
	payload_head = payload_head->next;
	mmw_process_mem_free((void**)&payload_head->payload);	//second is name buffer
}

payload_node_t* mmw_psic_debug_proto_report_add_payload_list(char *ptr_channel_name, uint8_t dim, uint16_t len, PsicDbgProtoPayloadDataFormat_e data_format, uint8_t signed_flag, uint8_t align_mode, uint8_t q_value, uint8_t *header, void *ptr_data)
{
#define GEN_WIDTH_BITS(x)	((x) & 0x03)
#define GEN_SIGN_BITS(x)	(((x) & 0x01) << 2)
#define GEN_Q_BITS(x)		(((x) & 0x1f) << 3)
#define GEN_ALIGN_MODE(x)	((x) & 0x01)

	PsicDegProtoHeader_t payload_header;
	uint32_t total_data_size;
	if (ptr_channel_name) {
        payload_header.len = len;
        payload_header.dim = dim;
        payload_header.align_mode = align_mode;
        payload_header.payload_data_format = data_format;
        if (signed_flag) {
            payload_header.payload_data_sign = PSIC_DBG_PROTO_DATA_SIGNED;
        } else {
            payload_header.payload_data_sign = PSIC_DBG_PROTO_DATA_UNSIGNED;
        }
        payload_header.q_format = q_value;
		payload_header.dim = payload_header.dim ? payload_header.dim : 1;
        switch (data_format) {
            case PSIC_DBG_PROTO_DATA_FORMAT_BYTE:
                total_data_size = len;
                break;
            case PSIC_DBG_PROTO_DATA_FORMAT_SHORT:
                total_data_size = len * 2;
                break;
            case PSIC_DBG_PROTO_DATA_FORMAT_LONG:
                total_data_size = len * 4;
                break;
            case PSIC_DBG_PROTO_DATA_FORMAT_FLOATING:
                total_data_size = len * 4;
                break;
            default:
                printf("error: unsupported type\r\n");
                return NULL;
                break;
        }
		total_data_size = total_data_size * payload_header.dim;
		header[0] = payload_header.dim;
		header[1] = GEN_WIDTH_BITS(payload_header.payload_data_format) |
						GEN_SIGN_BITS(payload_header.payload_data_sign) |
						GEN_Q_BITS(payload_header.q_format);
		header[2] = GEN_ALIGN_MODE(payload_header.align_mode);
		header[3] = payload_header.len & 0xff;
		header[4] = payload_header.len >> 8;
		payload_node_t *payload_head = NULL;

		payload_head = HIF_MsgReport_ListAddData(payload_head, header, 5);
		payload_head = HIF_MsgReport_ListAddData(payload_head, ptr_channel_name, strlen(ptr_channel_name) + 1);
        if(ptr_data) {
            payload_head = HIF_MsgReport_ListAddData(payload_head, ptr_data, total_data_size);
        }
		return payload_head;
	} else {
		return NULL;
	}
}

void mmw_psic_debug_proto_report(void *ptr_data, char *ptr_channel_name, uint8_t dim, uint16_t len, PsicDbgProtoPayloadDataFormat_e data_format, uint8_t signed_flag, uint8_t align_mode, uint8_t q_value)
{
	uint8_t header[5];
	
    HIF_MsgHdr_t msgHdr = {0x00};
	msgHdr.flag = (1<<2);
	msgHdr.msg_id = 0xC6;
	msgHdr.type = 2;
	if (ptr_channel_name)
	{
		payload_node_t *payload_head = NULL;
		payload_head = mmw_psic_debug_proto_report_add_payload_list(ptr_channel_name, dim, len, data_format, signed_flag, align_mode, q_value, header, ptr_data);
		HIF_MsgReport_ListStart(&msgHdr, payload_head, NULL);	//blocking mode callback function is NULL
	}
}

int8_t mmw_psic_debug_proto_report_async(void *ptr_data, char *ptr_channel_name, uint8_t dim, uint16_t len, PsicDbgProtoPayloadDataFormat_e data_format, uint8_t signed_flag, uint8_t align_mode, uint8_t q_value, HIF_MsgTrans_Callback sentCb)
{
	int8_t ret = HIF_ERRCODE_NOT_READY;
    uint8_t *header = 0;
	mmw_process_mem_alloc((void**)&header, sizeof(*header) * 5);	//if use non-blocking method,the header needs to allocate memory in the heap,the length is fixed at 5

    HIF_MsgHdr_t msgHdr = {0x00};
	msgHdr.flag = (1<<2);
	msgHdr.msg_id = 0xC6;
	msgHdr.type = 2;
	
	if (ptr_channel_name)
	{
		payload_node_t *payload_head = NULL;
		payload_head = mmw_psic_debug_proto_report_add_payload_list(ptr_channel_name, dim, len, data_format, signed_flag, align_mode, q_value, header, ptr_data);	
		ret = HIF_MsgReport_ListStart(&msgHdr, payload_head, sentCb);	// if need use non-blocking mode,should configure the callback function
	}
	if (ret) {	//return 0 indicates that the HIF task has been added successfully,otherwise the memory needs to be released
		mmw_process_mem_free((void**)&header);
	}
	return ret;
}
