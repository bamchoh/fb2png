cmake_minimum_required(VERSION 3.31)
include_guard(GLOBAL)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(TARGET_SYSROOT /home/bamchoh/rpi-sysroot)
set(TARGET_ARCHITECTURE aarch64-linux-gnu)
set(CMAKE_SYSROOT ${TARGET_SYSROOT})

# if you use other version of gcc and g++ than gcc/g++ 9, you must change the following variables
set(CMAKE_C_COMPILER /opt/cross-pi-gcc/bin/${TARGET_ARCHITECTURE}-gcc)
set(CMAKE_CXX_COMPILER /opt/cross-pi-gcc/bin/${TARGET_ARCHITECTURE}-g++)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -isystem=/usr/include -isystem=/usr/local/include -isystem=/usr/include/${TARGET_ARCHITECTURE}")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${TARGET_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE} -Wl,-rpath-link=${TARGET_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE}")
