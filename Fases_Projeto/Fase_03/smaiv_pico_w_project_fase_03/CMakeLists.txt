cmake_minimum_required(VERSION 3.13)

if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)

project(smaiv_pico_w_project_fase_03 C CXX ASM)
pico_sdk_init()

# Fonte programa principal
set(APP_SOURCES
    src/main.c
)

# Fontes biblioteca externa (OLED)
set(LIB_SSD1306_SOURCES
    lib/ssd1306/ssd1306.c
)

# Executável principal com TODOS os arquivos-fonte
add_executable(${PROJECT_NAME}
    ${APP_SOURCES}
    ${LIB_SSD1306_SOURCES}
)

# Adicionando os diretórios de include.
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

# Link de todas as bibliotecas necessárias.
target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_gpio
    hardware_adc
    hardware_i2c
)

# Configurações de saída
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${PROJECT_NAME})