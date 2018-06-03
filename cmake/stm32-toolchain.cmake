#
# stm32-toolchain cmake 
# Copyright (C) 2017 Amitesh Singh
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library;
# if not, see <http://www.gnu.org/licenses/>.
#

cmake_minimum_required(VERSION 3.5.1)
include(CMakeForceCompiler)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION   1)
set(CMAKE_SYSTEM_PROCESSOR cortex-m3)

find_program(ARM_CC arm-none-eabi-gcc
    ${TOOLCHAIN_DIR}/bin)
find_program(ARM_CXX arm-none-eabi-g++
            ${TOOLCHAIN_DIR}/bin)
find_program(ARM_OBJCOPY arm-none-eabi-objcopy
            ${TOOLCHAIN_DIR}/bin)
find_program(ARM_SIZE_TOOL arm-none-eabi-size
            ${TOOLCHAIN_DIR}/bin)
find_program(ARM_AS arm-none-eabi-as
            ${TOOLCHAIN_DIR}/bin)

find_program(ARM_LD arm-none-eabi-ld
             ${TOOLCHAIN_DIR}/bin)
find_program(ARM_OBJCOPY arm-none-eabi-objcopy
             ${TOOLCHAIN_DIR}/bin)
find_program(ARM_SIZE arm-none-eabi-size
             ${TOOLCHAIN_DIR}/bin)
find_program(ARM_STRIP arm-none-eabi-strip
             ${TOOLCHAIN_DIR}/bin)   

find_program(ST_FLASH st-flash)
find_program(ST_INFO st-info)
find_program(OPENOCD openocd)
find_program(STM32FLASH stm32flash)

CMAKE_FORCE_C_COMPILER(${ARM_CC} GNU)
CMAKE_FORCE_CXX_COMPILER(${ARM_CXX} GNU)
set(CMAKE_ASM_FLAGS ${CMAKE_ASM_FLAGS} "-mcpu=cortex-m3 -mthumb")

set(BASE_PATH "${${PROJECT_NAME}_SOURCE_DIR}")
set(SRC_PATH "${BASE_PATH}/src")
set(LIB_PATH "${BASE_PATH}/lib")

if (NOT DEFINED MCU)
  set(MCU STM32F1)
endif ()

add_definitions(-DSTM32F1)

set(STM32F1_FLAGS "-Os -ggdb -mcpu=cortex-m3 -mthumb -mthumb-interwork -msoft-float" CACHE STRING "")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-exceptions -Wall ${STM32F1_FLAGS} -std=c99 -fdata-sections -ffunction-sections" CACHE STRING "" )
set(CMAKE_CXX_FLAGS " -fdata-sections -ffunction-sections -fno-exceptions -Wall --std=gnu++14 ${STM32F1_FLAGS} -Wextra -Wshadow -Wredundant-decls " CACHE STRING "" )
# -lnosys --specs=rdimon.specs - removed
set(CMAKE_EXE_LINKER_FLAGS   " -flto -T ${CMAKE_SOURCE_DIR}/libopencm3.ld -nostartfiles -lopencm3_stm32f1 -lc -specs=nosys.specs -Wl,--gc-sections -Wl,--relax" CACHE STRING "")

#file(GLOB_RECURSE USER_SOURCES ./*.c ./*.cpp)
#target_include_directories(${NAME}.elf PRIVATE )
#target_link_libraries(${NAME}.elf opencm3_stm32f1)

include_directories(${LIBOPENCM3_DIR}/include)
link_directories(${LIBOPENCM3_DIR}/lib ${LIBOPENCM3_DIR}/lib/stm32/f1)
link_libraries(opencm3_stm32f1)

function(add_executable_stm32f1 NAME)
    add_executable(${NAME} ${ARGN})
    set_target_properties(${NAME} PROPERTIES OUTPUT_NAME "${NAME}.elf")
    set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${NAME}.bin")

   add_custom_command(OUTPUT ${NAME}.bin
#                      COMMAND ${ARM_STRIP} ${NAME}.elf
                      COMMAND ${CMAKE_OBJCOPY} -Obinary ${NAME}.elf ${NAME}.bin
                      COMMAND ${ARM_SIZE} ${NAME}.elf
                      DEPENDS ${NAME})
    add_custom_target(${NAME}-final ALL DEPENDS ${NAME}.bin)

    #add_custom_target(${NAME}.bin ALL COMMAND ${CMAKE_OBJCOPY} -Obinary ${NAME}.elf ${NAME}.bin)
    add_custom_target(${NAME}-size COMMAND ${ARM_SIZE} ${NAME}.elf)
    add_custom_target(${NAME}-probe COMMAND ${ST_INFO} --probe)
    add_custom_target(${NAME}-upload COMMAND ${ST_FLASH} write ${NAME}.bin 0x08000000)
    add_custom_target(${NAME}-ramupload COMMAND ${ST_FLASH} write ${NAME}.bin 0x20000000)
   # This is inspired from following video.   
   #https://www.youtube.com/watch?v=0eHpoPZvI3U&t=26s
   # /usr/share/openocd/target/stm32f103.cfg
    add_custom_target(${NAME}-ocdupload COMMAND ${OPENOCD} -f interface/stlink-v2.cfg -f target/stm32f103.cfg -c init -c "reset halt" -c "flash write_image erase ${NAME}.bin 0x08000000" -c "reset")
    add_custom_target(${NAME}-serialupload COMMAND sudo ${STM32FLASH} -w ${NAME}.bin ${SERIAL_PORT})

endfunction(add_executable_stm32f1)
