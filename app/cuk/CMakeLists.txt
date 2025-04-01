
add_library(cuk)

target_sources(cuk
    PRIVATE
    cukOpil.c

    cukController.c
    cukControlIdle.c
    cukControlRamp.c
    cukControlSfb.c
    cukControlEnergy.c
    cukControlCascFblin.c
    cukControlEnergyMpc.c

    cukHwOpil.c
    cukHwIf.c
)

target_link_libraries(cuk
    PRIVATE
    ocp
    opil
)

target_include_directories(cuk
    PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}"
)

target_include_directories(cuk
    PRIVATE
    "${PROJECT_SOURCE_DIR}"
    "${PROJECT_BINARY_DIR}/app"
)
