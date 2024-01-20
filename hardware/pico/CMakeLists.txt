cmake_minimum_required(VERSION 3.12)

#add_subdirectory(pico_ocp_link)

add_library(ocp_pico_hw wz_tcp_server.c ocpPicoCpu0.c wz_pico_init.c)

target_link_libraries(ocp_pico_hw pico_stdlib hardware_spi wiznet ocp)

target_include_directories(ocp_pico_hw PUBLIC "${PROJECT_SOURCE_DIR}")