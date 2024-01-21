cmake_minimum_required(VERSION 3.12)

add_library(ocp_ipc ipcClient.c ipcServer.c)

target_include_directories(ocp_ipc PUBLIC "${PROJECT_SOURCE_DIR}")