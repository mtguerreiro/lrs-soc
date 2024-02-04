cmake_minimum_required(VERSION 3.12)

add_library(ocp_app_it3903c itm3903cController.c itm3903cHwIf.c
    itm3903cControlOL.c itm3903cControlDisabled.c
    itm3903cHw.c itm3903cHwIf.c)

target_link_libraries(ocp_app_it3903c ocp_rp hardware_adc pico_stdlib)

target_include_directories(ocp_app_it3903c PUBLIC "${PROJECT_SOURCE_DIR}")