cmake_minimum_required(VERSION 3.16)
project(ApolloSaturnIB C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# The interactive DSKY/FDAI panel moved to the DSKY-matic project as
# dskymatic-studio.  The legacy apolloa target in this repo remains
# buildable for back-compat but the data files it reads (AGC ROM, PAD
# loads, downlink TSVs, panel scene, textures) now live in DSKY-matic;
# turning BUILD_GUI=ON here requires pointing at that data at runtime.
option(BUILD_GUI "Build the legacy apolloa interactive panel (deprecated; use dskymatic-studio)" OFF)

include(cmake/FetchVirtualAGC.cmake)

# ---------------------------------------------------------------------------
# LVDC rewrite (branch lvdc-rewrite; see docs/LVDC_REWRITE_PLAN.md).
# Work-in-progress replacement for the hand-edited native snapshot.
# Builds a separate static library; NOT linked into saturnib until
# Phase D swap-in. Off by default.
# ---------------------------------------------------------------------------
option(BUILD_LVDC_REWRITE "Build the lvdc_rewrite static library (work in progress)" OFF)
if(BUILD_LVDC_REWRITE)
    # enable_testing must precede any add_test in a subdirectory
    enable_testing()
    add_subdirectory(lib_SaturnIBObliq/native-snapshots/lvda-lvdc-rewrite)
endif()

# ---------------------------------------------------------------------------
# saturnib: pure launch-vehicle engine (Obliq plant + LVDC + IMU physics).
# Zero yaAGC dependency; batch_sim and tests link only this.
# LVDA_LVDC is compiled from native-snapshots/ (the hand-edited source).
# ---------------------------------------------------------------------------
set(PLANT_GEN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib_SaturnIBObliq/plant/generated)
set(LVDC_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/lib_SaturnIBObliq/native-snapshots/lvda-lvdc)

add_library(saturnib STATIC
    src/sim/SaturnIBPlantObliq.c
    src/sim/SaturnIBStack.cpp
    src/sim/SaturnIBStackAdapter.c
    src/sim/plant_serialize.cpp
    src/sim/igm_trace.c
    src/sim/igm_trace_stash.c
    src/sim/ic_trace.c
    src/sim/onpad_trace.c
    src/sim/sib_trace.c
    src/support/Date.cpp
    src/support/Log.cpp
    src/support/SimClock.cpp
    src/support/Vector.cpp
    src/support/save_state.cpp
    ${PLANT_GEN_DIR}/saturn_ib_stack.c
    ${LVDC_DIR}/LVDA_LVDC.c
)
target_include_directories(saturnib
    PUBLIC  src/sim src/support
    # The LVDA_LVDC.h in native-snapshots must win over the generated one.
    PRIVATE ${LVDC_DIR} ${PLANT_GEN_DIR}
)
target_link_libraries(saturnib PUBLIC m)

# ---------------------------------------------------------------------------
# saturnib_cmc: Saturn-IB ↔ CMC (yaAGC) bridge.  Contains all AGC coupling
# (uplink, downlink, channel handlers, PIPA/CDU injection).  Links yaAGC.
# apolloa links this AND saturnib; batch_sim does not.
# ---------------------------------------------------------------------------
add_library(saturnib_cmc STATIC
    src/cmc/SaturnIBAgcBridge.cpp
    src/cmc/SaturnStartupHelper.cpp
    src/cmc/UplinkMessage.cpp
)
target_include_directories(saturnib_cmc PUBLIC src/cmc)
target_link_libraries(saturnib_cmc PUBLIC saturnib yaAGC)

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
    target_link_libraries(apolloa PRIVATE saturnib saturnib_cmc OpenGL::GL OpenGL::GLU GLUT::GLUT)
endif()

# ---------------------------------------------------------------------------
# Tests (ctest).  The GUI scenario runs at wall-clock pace, so it is run by
# hand: see tests/gui/run_scenario.py.
# ---------------------------------------------------------------------------
enable_testing()
foreach(t determinism staging_events baseline chi_freeze)
    add_test(NAME ${t} COMMAND ${CMAKE_SOURCE_DIR}/tests/batch/${t}.sh $<TARGET_FILE:batch_sim>)
endforeach()

if(BUILD_GUI)
    find_package(X11)
    if(X11_FOUND)
        add_executable(xclick EXCLUDE_FROM_ALL tests/gui/xclick.c)
        target_link_libraries(xclick PRIVATE X11::X11)
    endif()
endif()
