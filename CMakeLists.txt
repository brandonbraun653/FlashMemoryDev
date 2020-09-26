cmake_minimum_required(VERSION 3.18.0)
project(FlashMemoryDev VERSION 0.1.0)
enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CPP_PLATFORM "freertos")

# ====================================================
# Perform very high level configuration
# ====================================================
# Set some vars common to both development environments
include("lib/CommonTools/cmake/options/common.cmake")

# Override the linker command
include("lib/CommonTools/cmake/toolchains/linker_whole_archive.cmake")

# Use our own FreeRTOSConfig.h
set(CUSTOM_FREERTOS_CFG ON CACHE INTERNAL "" FORCE)
add_library(freertos_cfg INTERFACE)
target_include_directories(freertos_cfg INTERFACE "config/freertos")
export(TARGETS freertos_cfg FILE "${PROJECT_BINARY_DIR}/ProjectConfig/freertos-cfg.cmake")


# ====================================================
# Import common packages and projects
# ====================================================
find_package(Boost REQUIRED)
add_subdirectory("lib/CommonTools/googletest")
add_subdirectory("lib/CommonTools/cpputest")
add_subdirectory("lib/Aurora")
add_subdirectory("lib/Chimera")
add_subdirectory("lib/FreeRTOS")
add_subdirectory("lib/Thor")
add_subdirectory("Flashmemory")
add_subdirectory("tests/common")

# ====================================================
# Select the group of targets to expose depending on
# the compiler. Some will be dedicated for on-device
# execution and some for running on the host PC.
# ====================================================
if(Toolchain::HAS_GCC_ARM_NONE_EABI)
  include("cmake/embedded_targets.cmake")
else()
  include("cmake/generic_targets.cmake")
endif()