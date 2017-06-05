#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.12)

set(sss_nnn_sources
    ./src/main.c
)

set(sss_nnn_sources 
    ${sss_nnn_sources}
    ./src/SssNnn.json
)
set_source_files_properties(./src/SssNnn.json PROPERTIES HEADER_FILE_ONLY ON)

include_directories(./inc ${IOTHUB_CLIENT_INC_FOLDER})
include_directories(${GW_INC})
include_directories(../../modules/common)

add_executable(sss_nnn_sample ${sss_nnn_sources})

# Add all necessary modules ! 'XxxYyy1 XxxYyy2 XxxYyy3' is just a sample 
add_dependencies(sss_nnn_sample sss_nnn XxxYyy1 XxxYyy2 XxxYyy3 logger)

# if you need other libraries, add them
target_link_libraries(sss_nnn_sample gateway nanomsg logger)

linkSharedUtil(sss_nnn_sample)
install_broker(sss_nnn_sample ${CMAKE_CURRENT_BINARY_DIR}/$(Configuration) )
copy_gateway_dll(sss_nnn_sample ${CMAKE_CURRENT_BINARY_DIR}/$(Configuration) )

add_sample_to_solution(sss_nnn_sample)
