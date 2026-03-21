#ifndef MMW_POINT_CLOUD_VV_TOOLS_H
#define MMW_POINT_CLOUD_VV_TOOLS_H

#include <stdint.h>
#include "mmw_ctrl.h"

void mmw_point_cloud_run_vv(const CFAR_Result *ptr_cfar_result, const CFAR_Info *ptr_cfar_info, uint16_t cfar_num);

void mmw_point_cloud_vv_upload_mcu_result(PointCloudBuffer_t *ptr_point_cloud_buffer);

#endif