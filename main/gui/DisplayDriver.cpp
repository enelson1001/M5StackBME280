/****************************************************************************************
 * DisplayDriver.h - A LittlevGL Display Driver for ILI9341
 * 
 * Created on Dec. 03, 2019
 * Copyright (c) 2019 Ed Nelson (https://github.com/enelson1001)
 * Licensed under MIT License (see LICENSE file)
 *
 * Derivative Works
 * Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
 * Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)
 * Licensed under the Apache License, Version 2.0 (the "License");
 *
 * LittlevGL - A powerful and easy-to-use embedded GUI
 * Copyright (c) 2016 Gábor Kiss-Vámosi (https://github.com/littlevgl/lvgl)
 * Licensed under MIT License
 ***************************************************************************************/
#include "gui/DisplayDriver.h"
#include <esp_freertos_hooks.h>
#include <smooth/core/io/spi/SpiDmaFixedBuffer.h>
#include <smooth/core/logging/log.h>

using namespace smooth::core::io::spi;
using namespace smooth::application::display;
using namespace smooth::core::logging;

namespace redstone
{
    // Class Constants
    static const char* TAG = "DisplayDriver";

    // Constructor
    DisplayDriver::DisplayDriver() :
            spi_host(VSPI_HOST),            // Use VSPI as host

            spi_master(
                spi_host,                   // host VSPI
                DMA_1,                      // use DMA
                GPIO_NUM_23,                // mosi gpio pin
                GPIO_NUM_19,                // miso gpio pin  (full duplex)
                GPIO_NUM_18,                // clock gpio pin
                MAX_DMA_LEN)                // max transfer size
    {
    }

    // Initialize the display driver
    bool DisplayDriver::initialize()
    {
        Log::info(TAG, "Initializing Lvgl Display Driver ........");

        ili9341_initialized = init_ILI9341();

        if (ili9341_initialized)
        {
            // M5Stack requires a different setting for a portrait and landscape screen
            // portrait = 0x68, landscape = 0x08 inverted landscape = 0xC8
            display->set_rotation(0x08);

            // initialize LittlevGL graphics library
            lv_init();

            // create a small video display buffer,  used by LittlevGL to draw screen content
            static SpiDmaFixedBuffer<uint8_t, MAX_DMA_LEN> video_display_buffer1;

            if (video_display_buffer1.is_buffer_allocated())
            {
                static lv_color1_t* vdb1 = reinterpret_cast<lv_color1_t*>(video_display_buffer1.data());

                // initialize a display buffer
                static lv_disp_buf_t disp_buf;
                lv_disp_buf_init(&disp_buf, vdb1, NULL, MAX_DMA_LEN / COLOR_SIZE);

                // initialize and register a display driver
                lv_disp_drv_t disp_drv;
                lv_disp_drv_init(&disp_drv);
                disp_drv.buffer = &disp_buf;
                disp_drv.flush_cb = ili9341_flush_cb;
                disp_drv.user_data = this;
                lv_disp_drv_register(&disp_drv);

                // LittlevGL graphics library's tick - runs every 1ms
                esp_register_freertos_tick_hook(lv_tick_task);
            }
            else
            {
                ili9341_initialized = false;
            }
        }

        return ili9341_initialized;
    }

    // Initialize the ILI9341
    bool DisplayDriver::init_ILI9341()
    {
        auto device = spi_master.create_device<ILI9341>(
                        GPIO_NUM_14,            // chip select gpio pin
                        GPIO_NUM_27,            // data command gpio pin
                        GPIO_NUM_33,            // reset gpio pin
                        GPIO_NUM_32,            // backlight gpio pin
                        0,                      // spi command_bits
                        0,                      // spi address_bits,
                        0,                      // bits_between_address_and_data_phase,
                        0,                      // spi_mode = 0,
                        128,                    // spi positive_duty_cycle,
                        0,                      // spi cs_ena_posttrans,
                        SPI_MASTER_FREQ_16M,    // spi-sck = 16MHz
                        0,                      // full duplex (4-wire)
                        7,                      // queue_size,
                        true,                   // use pre-trans callback
                        true);                  // use post-trans callback

        bool res = device->init(spi_host);

        if (res)
        {
            device->reset_display();
            res &= device->send_init_cmds();
            device->set_back_light(true);
            display = std::move(device);
        }
        else
        {
            Log::error(TAG, "Initializing of SPI Device: ILI9341 --- FAILED");
        }

        return res;
    }

    // A class instance callback to flush the display buffer and thereby write colors to screen
    void DisplayDriver::display_drv_flush(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_map)
    {
        uint32_t x1 = area->x1;
        uint32_t y1 = area->y1;
        uint32_t x2 = area->x2;
        uint32_t y2 = area->y2;

        uint32_t number_of_bytes_to_flush = (x2 - x1 + 1) * (y2 - y1 + 1) * COLOR_SIZE;
        uint32_t number_of_dma_blocks_with_complete_lines_to_send = number_of_bytes_to_flush / MAX_DMA_LEN;
        uint32_t number_of_bytes_in_not_complete_lines_to_send = number_of_bytes_to_flush % MAX_DMA_LEN;

        uint32_t start_row = y1;
        uint32_t end_row = y1 + LINES_TO_SEND - 1;

        // Drawing area that has a height of LINES_TO_SEND
        while (number_of_dma_blocks_with_complete_lines_to_send--)
        {
            display->send_lines(x1, start_row, x2, end_row, reinterpret_cast<uint8_t*>(color_map), MAX_DMA_LEN);
            display->wait_for_send_lines_to_finish();

            // color_map is pointer to type lv_color_t were the data type is based on color size so the
            // color_map pointer may have a data type of uint8_t or uint16_t or uint32_t.  MAX_DMA_LEN is
            // a number based on uint8_t so we need to divide MAX_DMA_LEN by the color size to increment
            // color_map pointer correctly
            color_map += MAX_DMA_LEN / COLOR_SIZE;

            // update start_row and end_row since we have sent a quantity of LINES_TO_SEND rows
            start_row = end_row + 1;
            end_row = start_row + LINES_TO_SEND - 1;
        }

        // Drawing area that has a height of less than LINE_TO_SEND
        if (number_of_bytes_in_not_complete_lines_to_send)
        {
            end_row = y2;
            display->send_lines(x1, start_row, x2, end_row,
                                reinterpret_cast<uint8_t*>(color_map),
                                number_of_bytes_in_not_complete_lines_to_send);

            display->wait_for_send_lines_to_finish();
        }

        // Inform the lvgl graphics library that we are ready for flushing the VDB buffer
        lv_disp_t* disp = lv_refr_get_disp_refreshing();
        lv_disp_flush_ready(&disp->driver);
    }

    // The "C" style callback required by LittlevGL
    void IRAM_ATTR DisplayDriver::ili9341_flush_cb(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_map)
    {
        DisplayDriver* driver = static_cast<DisplayDriver*>(drv->user_data);
        driver->display_drv_flush(drv, area, color_map);
    }

    // The  lv_tick_task that is required by LittlevGL for internal timing
    void IRAM_ATTR DisplayDriver::lv_tick_task(void)
    {
        lv_tick_inc(portTICK_RATE_MS);  // 1 ms tick_task
    }
}
