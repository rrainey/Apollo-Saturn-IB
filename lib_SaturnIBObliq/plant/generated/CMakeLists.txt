cmake_minimum_required(VERSION 3.16)
project(saturn_ib_stack_cgen C)
set(LVDC_SNAP "${CMAKE_CURRENT_SOURCE_DIR}/../../native-snapshots/lvda-lvdc")
add_library(saturn_ib_stack STATIC
  saturn_ib_stack.c
  ${LVDC_SNAP}/LVDA_LVDC.c
  ../../../src/sim/SaturnIBStackRtwAdapter.c
)
target_include_directories(saturn_ib_stack PUBLIC
  ${LVDC_SNAP}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/sim
)
target_compile_options(saturn_ib_stack PRIVATE -Wall -Wextra -O2)
target_link_libraries(saturn_ib_stack PUBLIC m)
add_executable(saturn_ib_stack_smoke smoke_main.c)
target_link_libraries(saturn_ib_stack_smoke PRIVATE saturn_ib_stack)
