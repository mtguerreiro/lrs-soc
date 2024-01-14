cmake_minimum_required(VERSION 3.12)

add_library(ocp_rp rp.c )

target_include_directories(ocp_rp PUBLIC "${PROJECT_SOURCE_DIR}")