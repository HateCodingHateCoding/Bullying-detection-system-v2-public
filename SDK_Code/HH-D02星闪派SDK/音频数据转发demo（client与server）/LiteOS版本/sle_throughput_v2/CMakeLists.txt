#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_SERVER_SPEED_V2)
    add_subdirectory_if_exist(sle_speed_server)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_CLIENT_SPEED_V2)
    add_subdirectory_if_exist(sle_speed_client)
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)
