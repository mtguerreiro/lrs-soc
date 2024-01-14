cmake_minimum_required(VERSION 3.12)

add_library(ocp_controlsys control.c controlsys.c)

target_include_directories(ocp_controlsys PUBLIC "${PROJECT_SOURCE_DIR}")