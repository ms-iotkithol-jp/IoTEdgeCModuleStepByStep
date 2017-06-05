cmake_minimum_required(VERSION 2.8.12)

set(XxxYyy_sources
    ./src/XxxYyy.c
)

set(XxxYyy_headers
    ./inc/XxxYyy.h
)

include_directories(./inc)
include_directories(${GW_INC})

#this builds the XxxYyy dynamic library
add_library(XxxYyy MODULE ${XxxYyy_sources} ${XxxYyy_headers})
target_link_libraries(XxxYyy gateway)

#this builds the XxxYyy static library
add_library(XxxYyy_static STATIC ${XxxYyy_sources} ${XxxYyy_headers})
target_compile_definitions(XxxYyy_static PRIVATE BUILD_MODULE_TYPE_STATIC)
target_link_libraries(XxxYyy_static gateway)

linkSharedUtil(XxxYyy)
linkSharedUtil(XxxYyy_static)
add_module_to_solution(XxxYyy)

if(install_modules)
    install(TARGETS XxxYyy LIBRARY DESTINATION "${LIB_INSTALL_DIR}/modules") 
endif()
