cmake_minimum_required(VERSION 3.12)

add_library(ocp_trace ctrace_mem.c ctrace.c)

target_include_directories(ocp_trace PUBLIC "${PROJECT_SOURCE_DIR}")