# GNSS-SDR is a Global Navigation Satellite System software-defined receiver.
# This file is part of GNSS-SDR.
#
# Copyright (C) 2010-2022  (see AUTHORS file for a list of contributors)
# SPDX-License-Identifier: GPL-3.0-or-later

if(USE_CMAKE_TARGET_SOURCES)
    add_library(security_gr_blocks STATIC)
    target_sources(security_gr_blocks
            PRIVATE
            galileo_osnma_worker.cc
            PUBLIC
            galileo_osnma_worker.h
            )
else()
    source_group(Headers FILES galileo_osnma_worker.h)
    add_library(security_gr_blocks galileo_osnma_worker.cc galileo_osnma_worker.h)
endif()

target_include_directories(security_gr_blocks
        PUBLIC
        ${GNSSSDR_SOURCE_DIR}/src/algorithms/libs
        )

target_link_libraries(security_gr_blocks
        PUBLIC
        Boost::headers
        Gnuradio::blocks
        security_libs_osnma
        PRIVATE
        core_receiver
        core_system_parameters
        Gflags::gflags
        Glog::glog
        Matio::matio
        Gnuradio::pmt
        )

target_include_directories(security_gr_blocks
        PUBLIC
        ${GNSSSDR_SOURCE_DIR}/src/core/interfaces
        )

if(GNURADIO_USES_STD_POINTERS)
    target_compile_definitions(security_gr_blocks
            PUBLIC -DGNURADIO_USES_STD_POINTERS=1
            )
endif()

if(USE_GENERIC_LAMBDAS)
    set(has_generic_lambdas HAS_GENERIC_LAMBDA=1)
    set(no_has_generic_lambdas HAS_GENERIC_LAMBDA=0)
    target_compile_definitions(security_gr_blocks
            PRIVATE
            "$<$<COMPILE_FEATURES:cxx_generic_lambdas>:${has_generic_lambdas}>"
            "$<$<NOT:$<COMPILE_FEATURES:cxx_generic_lambdas>>:${no_has_generic_lambdas}>"
            )
else()
    target_compile_definitions(security_gr_blocks
            PRIVATE
            -DHAS_GENERIC_LAMBDA=0
            )
endif()

if(USE_BOOST_BIND_PLACEHOLDERS)
    target_compile_definitions(security_gr_blocks
            PRIVATE
            -DUSE_BOOST_BIND_PLACEHOLDERS=1
            )
endif()

if(PMT_USES_BOOST_ANY)
    target_compile_definitions(security_gr_blocks
            PRIVATE
            -DPMT_USES_BOOST_ANY=1
            )
endif()

if(ENABLE_CLANG_TIDY)
    if(CLANG_TIDY_EXE)
        set_target_properties(security_gr_blocks
                PROPERTIES
                CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
                )
    endif()
endif()

set_property(TARGET security_gr_blocks
        APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        )
