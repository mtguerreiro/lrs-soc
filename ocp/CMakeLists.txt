cmake_minimum_required(VERSION 3.12)

add_library(ocp ocpCS.c ocpIf.c ocpPlatform.c ocpTrace.c ocpOpil.c)

target_link_libraries(ocp ocp_trace ocp_rp ocp_controlsys)

target_include_directories(ocp PUBLIC "${PROJECT_SOURCE_DIR}")