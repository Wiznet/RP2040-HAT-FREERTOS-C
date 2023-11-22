set(TARGET_NAME rp_eth_io_tests)

add_executable(${TARGET_NAME}
        main.c
        rp_eth_io/rp_eth_io_pins.c
        rp_eth_io/rp_eth_io_init.c
        rp_eth_io/rp_eth_io_adc.c
        rp_eth_io/rp_eth_io_i2c.c
        rp_eth_io/rp_eth_io_common.c

        tests.c
        )

target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/lib
        ${CMAKE_CURRENT_LIST_DIR}/rp_eth_io
        )

target_link_libraries(${TARGET_NAME} PRIVATE
        pico_stdlib
        hardware_spi
        hardware_dma
        FREERTOS_FILES
        ETHERNET_FILES
        IOLIBRARY_FILES
        DHCP_FILES
        DNS_FILES
        TIMER_FILES

        pico_multicore
        hardware_i2c
        hardware_timer
        hardware_flash
        hardware_sync
        hardware_adc
        hardware_dma
        hardware_uart
        hardware_irq
        )

pico_enable_stdio_uart(${TARGET_NAME} 0)
pico_enable_stdio_usb(${TARGET_NAME} 1)

pico_set_program_version(${TARGET_NAME} "1.0.0")

pico_add_extra_outputs(${TARGET_NAME})
