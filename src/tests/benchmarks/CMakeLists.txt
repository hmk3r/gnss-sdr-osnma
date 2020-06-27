# Copyright (C) 2010- 2020  (see AUTHORS file for a list of contributors)
#
# GNSS-SDR is a software-defined Global Navigation Satellite Systems receiver
#
# This file is part of GNSS-SDR.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

find_package(benchmark QUIET)

if(NOT benchmark_FOUND)
    set(BENCHMARK_COMPILER -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER})
    set(BENCHMARK_TOOLCHAIN_ARG "")
    if(DEFINED ENV{OECORE_TARGET_SYSROOT})
        set(BENCHMARK_COMPILER "")
        set(BENCHMARK_TOOLCHAIN_ARG "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_SOURCE_DIR}/cmake/Toolchains/oe-sdk_cross.cmake")
    else()
        if(CMAKE_TOOLCHAIN_FILE)
            set(BENCHMARK_TOOLCHAIN_ARG "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
        endif()
        if(ENABLE_PACKAGING)
            set(BENCHMARK_TOOLCHAIN_ARG "-DCMAKE_VERBOSE_MAKEFILE=ON")
        endif()
    endif()
    set(BENCHMARK_BUILD_COMMAND "${CMAKE_COMMAND}"
        "--build" "${CMAKE_BINARY_DIR}/benchmark-${GNSSSDR_BENCHMARK_LOCAL_VERSION}"
        "--config" $<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>$<$<CONFIG:MinSizeRel>:MinSizeRel>$<$<CONFIG:NoOptWithASM>:Debug>$<$<CONFIG:Coverage>:Debug>$<$<CONFIG:O2WithASM>:RelWithDebInfo>$<$<CONFIG:O3WithASM>:RelWithDebInfo>$<$<CONFIG:ASAN>:Debug>
    )
    if(CMAKE_GENERATOR STREQUAL Xcode)
        set(BENCHMARK_BUILD_COMMAND "xcodebuild" "-configuration" $<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>$<$<CONFIG:MinSizeRel>:MinSizeRel> "-target" "benchmark_main")
    endif()

    if((CMAKE_VERSION VERSION_GREATER 3.12.0) AND NOT (CMAKE_GENERATOR STREQUAL Xcode))
        set(BENCHMARK_PARALLEL_BUILD "--parallel 2")
    endif()

    ExternalProject_Add(google-benchmark-${GNSSSDR_BENCHMARK_LOCAL_VERSION}
        GIT_REPOSITORY https://github.com/google/benchmark
        GIT_TAG v${GNSSSDR_BENCHMARK_LOCAL_VERSION}
        SOURCE_DIR ${CMAKE_BINARY_DIR}/thirdparty/benchmark
        BINARY_DIR ${CMAKE_BINARY_DIR}/benchmark-${GNSSSDR_BENCHMARK_LOCAL_VERSION}
        CMAKE_ARGS ${BENCHMARK_COMPILER}
            -DBENCHMARK_ENABLE_GTEST_TESTS=OFF
            ${BENCHMARK_TOOLCHAIN_ARG}
            -DCMAKE_BUILD_TYPE=$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>$<$<CONFIG:MinSizeRel>:MinSizeRel>$<$<CONFIG:NoOptWithASM>:Debug>$<$<CONFIG:Coverage>:Debug>$<$<CONFIG:O2WithASM>:RelWithDebInfo>$<$<CONFIG:O3WithASM>:RelWithDebInfo>$<$<CONFIG:ASAN>:Debug>
        BUILD_COMMAND "${BENCHMARK_BUILD_COMMAND} ${BENCHMARK_PARALLEL_BUILD}"
        UPDATE_COMMAND ""
        PATCH_COMMAND ""
        BUILD_BYPRODUCTS ${CMAKE_BINARY_DIR}/benchmark-${GNSSSDR_BENCHMARK_LOCAL_VERSION}/src/${CMAKE_FIND_LIBRARY_PREFIXES}benchmark${CMAKE_STATIC_LIBRARY_SUFFIX}
        INSTALL_COMMAND ""
    )

    add_library(benchmark::benchmark STATIC IMPORTED)
    add_dependencies(benchmark::benchmark google-benchmark-${GNSSSDR_BENCHMARK_LOCAL_VERSION})
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/thirdparty/benchmark/include)
    set_target_properties(benchmark::benchmark PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
        IMPORTED_CONFIGURATIONS "None;Debug;Release;RelWithDebInfo;MinSizeRel"
        MAP_IMPORTED_CONFIG_NOOPTWITHASM Debug
        MAP_IMPORTED_CONFIG_COVERAGE Debug
        MAP_IMPORTED_CONFIG_O2WITHASM RelWithDebInfo
        MAP_IMPORTED_CONFIG_O3WITHASM RelWithDebInfo
        MAP_IMPORTED_CONFIG_ASAN Debug
        IMPORTED_LOCATION_NONE ${CMAKE_BINARY_DIR}/benchmark-${GNSSSDR_BENCHMARK_LOCAL_VERSION}/src/${CMAKE_FIND_LIBRARY_PREFIXES}benchmark${CMAKE_STATIC_LIBRARY_SUFFIX}
        IMPORTED_LOCATION_DEBUG ${CMAKE_BINARY_DIR}/benchmark-${GNSSSDR_BENCHMARK_LOCAL_VERSION}/src/${CMAKE_FIND_LIBRARY_PREFIXES}benchmark${CMAKE_STATIC_LIBRARY_SUFFIX}
        IMPORTED_LOCATION_RELEASE ${CMAKE_BINARY_DIR}/benchmark-${GNSSSDR_BENCHMARK_LOCAL_VERSION}/src/${CMAKE_FIND_LIBRARY_PREFIXES}benchmark${CMAKE_STATIC_LIBRARY_SUFFIX}
        IMPORTED_LOCATION_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/benchmark-${GNSSSDR_BENCHMARK_LOCAL_VERSION}/src/${CMAKE_FIND_LIBRARY_PREFIXES}benchmark${CMAKE_STATIC_LIBRARY_SUFFIX}
        IMPORTED_LOCATION_MINSIZEREL ${CMAKE_BINARY_DIR}/benchmark-${GNSSSDR_BENCHMARK_LOCAL_VERSION}/src/${CMAKE_FIND_LIBRARY_PREFIXES}benchmark${CMAKE_STATIC_LIBRARY_SUFFIX}
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_BINARY_DIR}/thirdparty/benchmark/include
        INTERFACE_LINK_LIBRARIES "${CMAKE_BINARY_DIR}/benchmark-${GNSSSDR_BENCHMARK_LOCAL_VERSION}/src/${CMAKE_FIND_LIBRARY_PREFIXES}benchmark${CMAKE_STATIC_LIBRARY_SUFFIX};${CMAKE_BINARY_DIR}/benchmark-${GNSSSDR_BENCHMARK_LOCAL_VERSION}/src/${CMAKE_FIND_LIBRARY_PREFIXES}benchmark_main${CMAKE_STATIC_LIBRARY_SUFFIX}"
    )
    if((CMAKE_GENERATOR STREQUAL Xcode) OR MSVC)
        set_target_properties(benchmark::benchmark PROPERTIES
            IMPORTED_LOCATION_DEBUG ${CMAKE_BINARY_DIR}/benchmark-${GNSSSDR_BENCHMARK_LOCAL_VERSION}/src/Debug/${CMAKE_FIND_LIBRARY_PREFIXES}benchmark${CMAKE_STATIC_LIBRARY_SUFFIX}
            IMPORTED_LOCATION_RELEASE ${CMAKE_BINARY_DIR}/benchmark-${GNSSSDR_BENCHMARK_LOCAL_VERSION}/src/Release/${CMAKE_FIND_LIBRARY_PREFIXES}benchmark${CMAKE_STATIC_LIBRARY_SUFFIX}
            IMPORTED_LOCATION_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/benchmark-${GNSSSDR_BENCHMARK_LOCAL_VERSION}/src/RelWithDebInfo/${CMAKE_FIND_LIBRARY_PREFIXES}benchmark${CMAKE_STATIC_LIBRARY_SUFFIX}
            IMPORTED_LOCATION_MINSIZEREL ${CMAKE_BINARY_DIR}/benchmark-${GNSSSDR_BENCHMARK_LOCAL_VERSION}/src/MinSizeRel/${CMAKE_FIND_LIBRARY_PREFIXES}benchmark${CMAKE_STATIC_LIBRARY_SUFFIX}
            INTERFACE_LINK_LIBRARIES "${CMAKE_BINARY_DIR}/benchmark-${GNSSSDR_BENCHMARK_LOCAL_VERSION}/src/$<$<CONFIG:Debug>:Debug/>$<$<CONFIG:Release>:Release/>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo/>$<$<CONFIG:MinSizeRel>:MinSizeRel/>benchmark${CMAKE_STATIC_LIBRARY_SUFFIX};${CMAKE_BINARY_DIR}/benchmark-${GNSSSDR_BENCHMARK_LOCAL_VERSION}/src/$<$<CONFIG:Debug>:Debug/>$<$<CONFIG:Release>:Release/>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo/>$<$<CONFIG:MinSizeRel>:MinSizeRel/>benchmark_main${CMAKE_STATIC_LIBRARY_SUFFIX}"
        )
    endif()
    set_target_properties(benchmark::benchmark PROPERTIES
        INTERFACE_LINK_LIBRARIES Threads::Threads
    )

endif()

macro(add_benchmark)
    # First argument is the benchmark name, the rest are dependencies
    add_executable(${ARGV0} ${ARGV0}.cc)
    target_link_libraries(${ARGV0} PRIVATE benchmark::benchmark)
    if(${ARGC} GREATER 1)
        set(list_dependencies "${ARGN}")
        list(REMOVE_AT list_dependencies 0 list_dependencies)
        foreach(dependency IN LISTS list_dependencies)
            target_link_libraries(${ARGV0} PRIVATE ${dependency})
        endforeach()
    endif()
    add_custom_command(TARGET ${ARGV0} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${ARGV0}>
            ${LOCAL_INSTALL_BASE_DIR}/install/$<TARGET_FILE_NAME:${ARGV0}>
    )
endmacro()

add_benchmark(benchmark_copy)
add_benchmark(benchmark_detector core_system_parameters)
add_benchmark(benchmark_preamble core_system_parameters)