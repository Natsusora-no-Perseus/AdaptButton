cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(main C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_compile_options(-Wall)

add_executable(main
    main.c
    serialsh.c
    sfuncs.c
)

target_link_libraries(main pico_stdlib pico_stdio_usb hardware_adc)

target_include_directories(main PRIVATE ${CMAKE_CURRENT_LIST_DIR})

pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 1)

pico_add_extra_outputs(main)
