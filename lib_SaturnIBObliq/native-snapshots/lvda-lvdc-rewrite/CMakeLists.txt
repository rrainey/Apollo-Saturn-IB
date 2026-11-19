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
    lvdc_preigm.c
    lvdc_minor_loop.c
    lvdc_chi_to_psi.c
    lvdc_dac.c
    lvdc_igm_intermediate.c
    lvdc_igm_phasing.c
    lvdc_igm_frames.c
    lvdc_igm_steering.c
    lvdc_igm_position_correction.c
    lvdc_igm_output.c
    lvdc_igm.c
    lvdc_moverf.c
    lvdc_timebase.c
    lvdc_hsl.c
    lvdc_plant_shim.c
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

add_executable(test_preigm_smoke tests/test_preigm_smoke.c)
target_link_libraries(test_preigm_smoke PRIVATE lvdc_rewrite)
target_compile_options(test_preigm_smoke PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow -Wmissing-prototypes
)
add_test(NAME lvdc_rewrite_preigm_smoke COMMAND test_preigm_smoke)

add_executable(test_output_pipeline_smoke tests/test_output_pipeline_smoke.c)
target_link_libraries(test_output_pipeline_smoke PRIVATE lvdc_rewrite)
target_compile_options(test_output_pipeline_smoke PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow -Wmissing-prototypes
)
add_test(NAME lvdc_rewrite_output_pipeline_smoke
         COMMAND test_output_pipeline_smoke)

add_executable(test_chi_to_psi_smoke tests/test_chi_to_psi_smoke.c)
target_link_libraries(test_chi_to_psi_smoke PRIVATE lvdc_rewrite)
target_compile_options(test_chi_to_psi_smoke PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow -Wmissing-prototypes
)
add_test(NAME lvdc_rewrite_chi_to_psi_smoke COMMAND test_chi_to_psi_smoke)

add_executable(test_igm_intermediate_smoke tests/test_igm_intermediate_smoke.c)
target_link_libraries(test_igm_intermediate_smoke PRIVATE lvdc_rewrite)
target_compile_options(test_igm_intermediate_smoke PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow -Wmissing-prototypes
)
add_test(NAME lvdc_rewrite_igm_intermediate_smoke
         COMMAND test_igm_intermediate_smoke)

add_executable(test_igm_phasing_smoke tests/test_igm_phasing_smoke.c)
target_link_libraries(test_igm_phasing_smoke PRIVATE lvdc_rewrite)
target_compile_options(test_igm_phasing_smoke PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow -Wmissing-prototypes
)
add_test(NAME lvdc_rewrite_igm_phasing_smoke
         COMMAND test_igm_phasing_smoke)

add_executable(test_igm_frames_smoke tests/test_igm_frames_smoke.c)
target_link_libraries(test_igm_frames_smoke PRIVATE lvdc_rewrite)
target_compile_options(test_igm_frames_smoke PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow -Wmissing-prototypes
)
add_test(NAME lvdc_rewrite_igm_frames_smoke
         COMMAND test_igm_frames_smoke)

add_executable(test_igm_steering_smoke tests/test_igm_steering_smoke.c)
target_link_libraries(test_igm_steering_smoke PRIVATE lvdc_rewrite)
target_compile_options(test_igm_steering_smoke PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow -Wmissing-prototypes
)
add_test(NAME lvdc_rewrite_igm_steering_smoke
         COMMAND test_igm_steering_smoke)

add_executable(test_igm_position_correction_smoke
               tests/test_igm_position_correction_smoke.c)
target_link_libraries(test_igm_position_correction_smoke PRIVATE lvdc_rewrite)
target_compile_options(test_igm_position_correction_smoke PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow -Wmissing-prototypes
)
add_test(NAME lvdc_rewrite_igm_position_correction_smoke
         COMMAND test_igm_position_correction_smoke)

add_executable(test_igm_output_smoke tests/test_igm_output_smoke.c)
target_link_libraries(test_igm_output_smoke PRIVATE lvdc_rewrite)
target_compile_options(test_igm_output_smoke PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow -Wmissing-prototypes
)
add_test(NAME lvdc_rewrite_igm_output_smoke
         COMMAND test_igm_output_smoke)

add_executable(test_igm_orchestrator_smoke
               tests/test_igm_orchestrator_smoke.c)
target_link_libraries(test_igm_orchestrator_smoke PRIVATE lvdc_rewrite)
target_compile_options(test_igm_orchestrator_smoke PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow -Wmissing-prototypes
)
add_test(NAME lvdc_rewrite_igm_orchestrator_smoke
         COMMAND test_igm_orchestrator_smoke)

add_executable(test_igm_presets_smoke tests/test_igm_presets_smoke.c)
target_link_libraries(test_igm_presets_smoke PRIVATE lvdc_rewrite)
target_compile_options(test_igm_presets_smoke PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow -Wmissing-prototypes
)
add_test(NAME lvdc_rewrite_igm_presets_smoke
         COMMAND test_igm_presets_smoke)

add_executable(test_moverf_smoke tests/test_moverf_smoke.c)
target_link_libraries(test_moverf_smoke PRIVATE lvdc_rewrite)
target_compile_options(test_moverf_smoke PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow -Wmissing-prototypes
)
add_test(NAME lvdc_rewrite_moverf_smoke
         COMMAND test_moverf_smoke)

add_executable(test_timebase_smoke tests/test_timebase_smoke.c)
target_link_libraries(test_timebase_smoke PRIVATE lvdc_rewrite)
target_compile_options(test_timebase_smoke PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow -Wmissing-prototypes
)
add_test(NAME lvdc_rewrite_timebase_smoke
         COMMAND test_timebase_smoke)

add_executable(test_hsl_smoke tests/test_hsl_smoke.c)
target_link_libraries(test_hsl_smoke PRIVATE lvdc_rewrite)
target_compile_options(test_hsl_smoke PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow -Wmissing-prototypes
)
add_test(NAME lvdc_rewrite_hsl_smoke
         COMMAND test_hsl_smoke)
