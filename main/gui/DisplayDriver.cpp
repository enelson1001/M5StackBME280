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
#include <smooth/core/logging/log.h>

using namespace smooth::core::io::spi;
using namespace smooth::application::display;
using namespace smooth::core::logging;
using namespace std::chrono;

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
            // set screen rotation
            set_screen_rotation();

            // initialize LittlevGL graphics library
            lv_init();

            // The video_display_buffer1 is used by LittlevGL to draw screen content
            // Verify that video_display_buffer1 has been created.
            if (video_display_buffer1.is_buffer_allocated())
            {
                // initialize a display buffer
                vdb1 = reinterpret_cast<lv_color1_t*>(video_display_buffer1.data());
                lv_disp_buf_init(&disp_buf, vdb1, NULL, MAX_DMA_LEN / COLOR_SIZE);

                // initialize and register a display driver
                lv_disp_drv_init(&disp_drv);
                disp_drv.buffer = &disp_buf;
                disp_drv.flush_cb = ili9341_flush_cb;
                disp_drv.user_data = this;
                lv_disp_drv_register(&disp_drv);
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
        auto device = spi_master.create_device<DisplaySpi>(
                        GPIO_NUM_14,            // chip select gpio pin
                        GPIO_NUM_27,            // data command gpio pin
                        0,                      // spi command_bits
                        0,                      // spi address_bits,
                        0,                      // bits_between_address_and_data_phase,
                        0,                      // spi_mode = 0,
                        128,                    // spi positive_duty_cycle,
                        0,                      // spi cs_ena_posttrans,
                        SPI_MASTER_FREQ_40M,    // spi-sck = 40MHz; since we are only writing to display
                        0,                      // full duplex (4-wire)
                        7,                      // queue_size,
                        true,                   // use pre-trans callback
                        true);                  // use post-trans callback

        bool res = device->init(spi_host);

        if (res)
        {
            // add reset pin - pullup=false, pulldown=false, active_high=false
            device->add_reset_pin(std::make_unique<DisplayPin>(GPIO_NUM_33, false, false, false));
            device->hw_reset(true, milliseconds(5), milliseconds(150));  // reset chip

            // add backlight pin - pullup=false, pulldown=false, active_high=true
            device->add_backlight_pin(std::make_unique<DisplayPin>(GPIO_NUM_32, false, false, true));
            device->set_back_light(true);  // turn on backlighting

            // initialize the display
            res &= device->send_init_cmds(ili9341_init_cmds_1.data(), ili9341_init_cmds_1.size());
            display = std::move(device);
        }
        else
        {
            Log::error(TAG, "Initializing of SPI Device: ILI9341 --- FAILED");
        }

        return res;
    }

    // Set screen rotation
    void DisplayDriver::set_screen_rotation()
    {
        bool res = true;

        // The default screen oreintation via init_cmds is portrait but the M5Stack requires
        // a different setting for a portrait and landscape. portrait = 0x68, landscape = 0x08
        // check the lv_config file for the screen oreintation
        if (LV_VER_RES_MAX > LV_HOR_RES_MAX)
        {
            // Portrait
            res = display->set_madctl(0x68);
        }
        else
        {
            // Landscape
            res = display->set_madctl(0x08);
        }

        if (!res)
        {
            Log::error(TAG, "Setting screen rotation --- FAILED");
        }
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
        DisplayDriver* driver = reinterpret_cast<DisplayDriver*>(drv->user_data);
        driver->display_drv_flush(drv, area, color_map);
    }
}
