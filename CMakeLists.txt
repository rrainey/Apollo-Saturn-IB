cmake_minimum_required(VERSION 3.16)
project(ApolloSaturnIB C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(BUILD_GUI "Build the apolloa interactive panel (needs OpenGL and GLUT)" ON)

include(cmake/FetchVirtualAGC.cmake)

# ---------------------------------------------------------------------------
# saturnib: Obliq plant, host-side vehicle systems, and support code.
# LVDA_LVDC is compiled from native-snapshots/ (the hand-edited source), not
# from plant/generated/.  See lib_SaturnIBObliq/README.md.
# ---------------------------------------------------------------------------
set(PLANT_GEN_DIR ${CMAKE_SOURCE_DIR}/lib_SaturnIBObliq/plant/generated)
set(LVDC_DIR      ${CMAKE_SOURCE_DIR}/lib_SaturnIBObliq/native-snapshots/lvda-lvdc)

add_library(saturnib STATIC
    src/sim/SaturnIBPlantObliq.c
    src/sim/SaturnIBStack.cpp
    src/sim/SaturnIBStackRtwAdapter.c
    src/sim/SaturnStartupHelper.cpp
    src/sim/UplinkMessage.cpp
    src/sim/igm_trace.c
    src/sim/igm_trace_stash.c
    src/sim/ic_trace.c
    src/sim/onpad_trace.c
    src/sim/sib_trace.c
    src/support/Date.cpp
    src/support/Log.cpp
    src/support/SimClock.cpp
    src/support/Vector.cpp
    ${PLANT_GEN_DIR}/saturn_ib_stack.c
    ${LVDC_DIR}/LVDA_LVDC.c
)
target_include_directories(saturnib
    PUBLIC  src/sim src/support
    # The LVDA_LVDC.h in native-snapshots must win over the generated one.
    PRIVATE ${LVDC_DIR} ${PLANT_GEN_DIR}
)
target_link_libraries(saturnib PUBLIC yaAGC m)

# ---------------------------------------------------------------------------
# batch_sim: headless driver
# ---------------------------------------------------------------------------
add_executable(batch_sim src/batch/main.cpp)
target_include_directories(batch_sim PRIVATE third_party)
target_link_libraries(batch_sim PRIVATE saturnib)

# ---------------------------------------------------------------------------
# apolloa: interactive DSKY + FDAI panel
# ---------------------------------------------------------------------------
if(BUILD_GUI)
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)

    add_executable(apolloa
        src/gui/apolloa_main.cpp
        src/gui/panel_pick.cpp
        src/gui/panel_render.cpp
        src/gui/panel_scene.cpp
        src/gui/panel_texture.cpp
        src/gui/stb_image_impl.cpp
    )
    target_include_directories(apolloa PRIVATE src/gui third_party third_party/stb)
    target_link_libraries(apolloa PRIVATE saturnib OpenGL::GL OpenGL::GLU GLUT::GLUT)
endif()

# ---------------------------------------------------------------------------
# Tests (ctest).  The GUI scenario runs at wall-clock pace, so it is run by
# hand: see tests/gui/run_scenario.py.
# ---------------------------------------------------------------------------
enable_testing()
foreach(t determinism staging_events baseline)
    add_test(NAME ${t} COMMAND ${CMAKE_SOURCE_DIR}/tests/batch/${t}.sh $<TARGET_FILE:batch_sim>)
endforeach()

if(BUILD_GUI)
    find_package(X11)
    if(X11_FOUND)
        add_executable(xclick EXCLUDE_FROM_ALL tests/gui/xclick.c)
        target_link_libraries(xclick PRIVATE X11::X11)
    endif()
endif()
