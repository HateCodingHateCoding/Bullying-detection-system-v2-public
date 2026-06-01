#ifndef RADAR_SLE_CLIENT_H
#define RADAR_SLE_CLIENT_H

#include "sle_ssap_client.h"

void radar_sle_client_init(ssapc_notification_callback notification_cb, ssapc_indication_callback indication_cb);
void radar_sle_start_scan(void);

#endif
