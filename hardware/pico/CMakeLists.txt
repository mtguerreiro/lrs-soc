cmake_minimum_required(VERSION 3.12)

add_library(ocp_pico_hw wz_tcp_server.c ocpPicoCpu0.c ocpPicoCpu1.c wz_pico_init.c ipcClientPico.c ipcServerPico.c)

target_link_libraries(ocp_pico_hw hardware_adc pico_stdlib hardware_spi pico_multicore wiznet ocp ocp_ipc)

target_include_directories(ocp_pico_hw PUBLIC "${PROJECT_SOURCE_DIR}")