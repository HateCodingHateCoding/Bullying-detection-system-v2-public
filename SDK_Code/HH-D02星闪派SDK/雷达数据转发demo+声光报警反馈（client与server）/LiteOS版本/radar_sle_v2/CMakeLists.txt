#===============================================================================
# @brief    Radar SLE v2 sample.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_RADAR_SLE_SERVER_V2)
    add_subdirectory_if_exist(radar_sle_server)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_RADAR_SLE_CLIENT_V2)
    add_subdirectory_if_exist(radar_sle_client)
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)
