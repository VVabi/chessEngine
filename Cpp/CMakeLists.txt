cmake_minimum_required(VERSION 3.21)

project(Vabi LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(VABI_HASH               "Enable hash tables (HASH)"                        ON)
option(VABI_RANDOMEVAL         "Add random noise to the eval (RANDOMEVAL)"        OFF)
option(VABI_EXPERIMENTAL       "Enable experimental features (EXPERIMENTAL)"      OFF)
option(VABI_UNROLL_LOOPS       "Compile with -funroll-loops"                      OFF)
option(VABI_WARNINGS_AS_ERRORS "Treat compiler warnings as errors"                ON)
option(VABI_PEDANTIC           "Compile with -pedantic -pedantic-errors"          OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Flags matching the former Eclipse CDT configurations
set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

file(GLOB_RECURSE VABI_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(Vabi ${VABI_SOURCES})

target_include_directories(Vabi PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_compile_options(Vabi PRIVATE -Wall -Wextra -fmessage-length=0)

target_compile_definitions(Vabi PRIVATE
    $<$<BOOL:${VABI_HASH}>:HASH>
    $<$<BOOL:${VABI_RANDOMEVAL}>:RANDOMEVAL>
    $<$<BOOL:${VABI_EXPERIMENTAL}>:EXPERIMENTAL>
    $<$<CONFIG:Debug>:DEBUG>
)

if(VABI_WARNINGS_AS_ERRORS)
    # Diagnostics added/extended in newer GCCs than the code was written against;
    # keep them visible but non-fatal.
    target_compile_options(Vabi PRIVATE -Werror
        -Wno-error=class-memaccess
        -Wno-error=maybe-uninitialized
        -Wno-error=uninitialized)
endif()

if(VABI_PEDANTIC)
    target_compile_options(Vabi PRIVATE -pedantic -pedantic-errors)
endif()

if(VABI_UNROLL_LOOPS)
    target_compile_options(Vabi PRIVATE -funroll-loops)
endif()

if(WIN32)
    target_compile_definitions(Vabi PRIVATE WINDOWS)
    target_link_options(Vabi PRIVATE -static-libgcc -static-libstdc++)
else()
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(Vabi PRIVATE Threads::Threads)
endif()
