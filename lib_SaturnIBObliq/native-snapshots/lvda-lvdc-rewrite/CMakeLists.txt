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

# ---------------------------------------------------------------------------
# Smoke tests. Built when BUILD_LVDC_REWRITE is on. Each test is also
# registered with ctest.
# ---------------------------------------------------------------------------
add_executable(test_nav_smoke tests/test_nav_smoke.c)
target_link_libraries(test_nav_smoke PRIVATE lvdc_rewrite)
target_compile_options(test_nav_smoke PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow -Wmissing-prototypes
)
add_test(NAME lvdc_rewrite_nav_smoke COMMAND test_nav_smoke)

add_executable(test_abi_smoke tests/test_abi_smoke.c)
target_link_libraries(test_abi_smoke PRIVATE lvdc_rewrite)
target_compile_options(test_abi_smoke PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow -Wmissing-prototypes
)
add_test(NAME lvdc_rewrite_abi_smoke COMMAND test_abi_smoke)

add_executable(replay_lvdc_trace tests/replay_lvdc_trace.c)
target_link_libraries(replay_lvdc_trace PRIVATE lvdc_rewrite)
target_compile_options(replay_lvdc_trace PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow -Wmissing-prototypes
)
# No ctest: needs a prerecorded trace CSV from batch_sim.
