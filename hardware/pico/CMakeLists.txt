cmake_minimum_required(VERSION 3.12)

add_library(ocp_pico_hw wz_tcp_server.c ocpPicoCpu0.c ocpPicoCpu1.c wz_pico_init.c ipcClientPico.c ipcServerPico.c mcp49x2.c)

target_link_libraries(ocp_pico_hw pico_stdlib hardware_adc hardware_spi pico_multicore pico_sync pico_unique_id wiznet ocp ocp_ipc ocp_app_it3903c)

target_include_directories(ocp_pico_hw PUBLIC "${PROJECT_SOURCE_DIR}")