# Clones Virtual AGC at a pinned commit and applies our embedding patches.
# The clone is sparse (yaAGC/ only); run `git sparse-checkout disable` inside
# it to get the whole project.
#
# To move to a newer Virtual AGC: change VIRTUALAGC_COMMIT, refresh the patch
# in third_party/virtualagc-patches/, delete third_party/virtualagc/, reconfigure.

set(VIRTUALAGC_URL "https://github.com/virtualagc/virtualagc.git" CACHE STRING "Virtual AGC repository")
set(VIRTUALAGC_COMMIT "9a60b41bad3156caeb191c7718652d57dba639b3")
set(VIRTUALAGC_PATCH "${CMAKE_SOURCE_DIR}/third_party/virtualagc-patches/yaagc-embed-tweaks.patch")
set(VIRTUALAGC_DIR "${CMAKE_SOURCE_DIR}/third_party/virtualagc")

find_package(Git REQUIRED)

function(_vagc_git)
    execute_process(COMMAND ${GIT_EXECUTABLE} ${ARGN}
                    WORKING_DIRECTORY ${VIRTUALAGC_DIR}
                    RESULT_VARIABLE rc)
    if(NOT rc EQUAL 0)
        message(FATAL_ERROR "git ${ARGN} failed in ${VIRTUALAGC_DIR}")
    endif()
endfunction()

file(SHA256 "${VIRTUALAGC_PATCH}" _patch_hash)
set(_stamp "${VIRTUALAGC_DIR}/.apollo-saturn-ib-stamp")
set(_stamp_value "${VIRTUALAGC_COMMIT} ${_patch_hash}")
set(_current "")
if(EXISTS "${_stamp}")
    file(READ "${_stamp}" _current)
endif()

if(NOT _current STREQUAL _stamp_value)
    message(STATUS "Fetching Virtual AGC ${VIRTUALAGC_COMMIT}")
    file(REMOVE_RECURSE "${VIRTUALAGC_DIR}")
    file(MAKE_DIRECTORY "${VIRTUALAGC_DIR}")
    _vagc_git(init -q)
    _vagc_git(remote add origin ${VIRTUALAGC_URL})
    _vagc_git(sparse-checkout set yaAGC)
    _vagc_git(fetch -q --depth 1 --filter=blob:none origin ${VIRTUALAGC_COMMIT})
    _vagc_git(checkout -q --detach FETCH_HEAD)
    _vagc_git(apply ${VIRTUALAGC_PATCH})
    file(WRITE "${_stamp}" "${_stamp_value}")
endif()

add_subdirectory("${VIRTUALAGC_DIR}/yaAGC" yaAGC_build)
