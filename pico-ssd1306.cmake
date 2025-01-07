add_library(pico_ssd1306 STATIC)
target_sources(pico_ssd1306 PUBLIC
    ${PICO_SSD1306_DIR}/ssd1306.c
)

# Add include directory
target_include_directories(pico_ssd1306 PUBLIC ${PICO_SSD1306_DIR})

# Add the standard library to the build
target_link_libraries(pico_ssd1306 PUBLIC pico_stdlib hardware_i2c)