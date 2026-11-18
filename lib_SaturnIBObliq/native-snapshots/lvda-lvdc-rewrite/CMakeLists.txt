cmake_minimum_required(VERSION 3.16)

# LVDC rewrite static library. Built only when BUILD_LVDC_REWRITE is on.
# Not linked into saturnib until Phase D swap-in.
#
# See docs/LVDC_REWRITE_PLAN.md and ../README.md in this directory.

add_library(lvdc_rewrite STATIC
    LVDA_LVDC.c
    lvdc_frames.c
    lvdc_quant.c
    lvdc_nav.c
    lvdc_params.c
)

target_include_directories(lvdc_rewrite
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(lvdc_rewrite PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow -Wmissing-prototypes
    -O2
)

target_link_libraries(lvdc_rewrite PUBLIC m)
