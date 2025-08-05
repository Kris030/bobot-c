cmake_minimum_required(VERSION 3.13)

set(PROJECT_NAME "bobot")

# needed for LSP support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# initialize pico-sdk from GIT
set(PICO_SDK_FETCH_FROM_GIT ON)

# TODO: remove when sdk build fixed
set(PICO_SDK_FETCH_FROM_GIT_TAG develop)
unset(ENV{PICO_SDK_PATH})

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

# TODO: pico2_w
set(PICO_BOARD pico2_w)

include(pico_sdk_import.cmake)

# initialize the Raspberry Pi Pico SDK
project(${PROJECT_NAME} C CXX ASM)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(
  ${PROJECT_NAME}
  src/rgb_sensor/impl/rgb_sensor.c
  src/servo/impl/servo.c
  src/h_bridge/impl/h_bridge.c
  src/net/impl/net.c
  src/net/impl/net_debug.c
  src/net/impl/net_mapman.c
  src/mapman/impl/mapman.c
  src/pid/impl/pid.c
  src/debug/impl/debug.c
  src/pwm/impl/pwm.c
  src/util/impl/util.c
  src/rgb_led/impl/rgb_led.c
  src/main.c
  src/bobot/impl/bobot.c
  src/encoder/impl/encoder.c
  src/ultra/impl/ultra.c #
)

# needed for serial USB communication
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${PROJECT_NAME})

# include directories for headers
target_include_directories(${PROJECT_NAME} PRIVATE src)

# pull in pico libraries that we need
target_link_libraries(
  ${PROJECT_NAME}
  pico_stdlib #
  pico_cyw43_arch_lwip_threadsafe_background #
  hardware_pwm #
  hardware_clocks #
  hardware_i2c #
)

target_compile_definitions(
  ${PROJECT_NAME}
  PRIVATE PICO_CYW43_ARCH_DEFAULT_COUNTRY_CODE=CYW43_COUNTRY_HUNGARY #
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

pico_set_program_name(${PROJECT_NAME} "bobot-ref")
pico_set_program_description(${PROJECT_NAME} "Obstacle course reference impl")
pico_set_program_url(${PROJECT_NAME} "https://github.com/kris030/bobot-c")
