# GNSS-SDR is a Global Navigation Satellite System software-defined receiver.
# This file is part of GNSS-SDR.
#
# Copyright (C) 2010-2022  (see AUTHORS file for a list of contributors)
# SPDX-License-Identifier: GPL-3.0-or-later

file(GLOB OSNMA_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB OSNMA_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")


list(SORT OSNMA_HEADERS)
list(SORT OSNMA_SOURCES)

if(USE_CMAKE_TARGET_SOURCES)
    add_library(security_libs_osnma STATIC)
    target_sources(security_libs_osnma
            PRIVATE
            ${OSNMA_SOURCES}
            PUBLIC
            ${OSNMA_HEADERS}
            )
else()
    source_group(Headers FILES ${OSNMA_HEADERS})
    add_library(security_libs_osnma
            STATIC
            ${OSNMA_SOURCES}
            ${OSNMA_HEADERS}
            )
endif()

# TODO: Add openssl fallback
#if(OPENSSL_FOUND)
#    target_compile_definitions(security_libs_osnma PUBLIC -DUSE_OPENSSL_FALLBACK=1)
#endif()

# Temporary workaround for distros that don't ship gnutls-openssl
# The root CMAKE file overwrites GNUTLS_INCLUDE_DIR and GNUTLS_LIBRARIES when GNUTLS_OPENSSL is not found,
# So GNUTLS becomes shadowed by OPENSSL. When the openssl fallback is implemented, this won't be an issue
set(TMP_GNUTLS_INCLUDE_DIR ${GNUTLS_INCLUDE_DIR})
set(TMP_GNUTLS_LIBRARIES ${GNUTLS_LIBRARIES})
find_package(GnuTLS)
set_package_properties(GnuTLS PROPERTIES
        URL "https://www.gnutls.org/"
        PURPOSE "Used for the OSNMA protocol implementation."
        TYPE REQUIRED
        )


find_package(Boost COMPONENTS system filesystem REQUIRED)

target_link_libraries(security_libs_osnma
        PUBLIC
        ${GNUTLS_LIBRARIES}
        PRIVATE
        Pugixml::pugixml
        Boost::headers
        Boost::system
        Boost::filesystem
        )

target_include_directories(security_libs_osnma
        PUBLIC
        ${GNUTLS_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

if(ENABLE_CLANG_TIDY)
    if(CLANG_TIDY_EXE)
        set_target_properties(security_libs_osnma
                PROPERTIES
                CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
                )
    endif()
endif()

set_property(TARGET security_libs_osnma
        APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${GNUTLS_INCLUDE_DIR}>
        )

set_target_properties(security_libs_osnma PROPERTIES LINKER_LANGUAGE CXX)

set(GNUTLS_INCLUDE_DIR ${TMP_GNUTLS_INCLUDE_DIR})
set(GNUTLS_LIBRARIES ${TMP_GNUTLS_LIBRARIES})