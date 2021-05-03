/****************************************************************************************
 * App.cpp - The Application class
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

//*****************************************************************************************************************
// Typical output on M5Stack (after running for 8 hours)
//
//  MemStat: Mem type |  8-bit free | Smallest block | Minimum free | 32-bit free | Smallest block | Minimum free
//  MemStat: INTERNAL |       68496 |          32768 |        12432 |       96588 |          32768 |        40512
//  MemStat:      DMA |       68496 |          32768 |        12432 |       68496 |          32768 |        12432
//  MemStat:   SPIRAM |           0 |              0 |            0 |           0 |              0 |            0
//  MemStat:
//  MemStat:             Name |      Stack |  Min free stack |  Max used stack
//  MemStat:         MainTask |      16384 |           12068 |            4316
//  MemStat:   PollSensorTask |       3300 |             300 |            3000
//  MemStat:         SntpTask |       3200 |             920 |            2280
//  MemStat: SocketDispatcher |      20480 |           18028 |            2452
//  MemStat:         LvglTask |       4096 |             476 |            3620
//
// Esp32-IDF version: v4.3-beta3 - commit e9cf9e2 - April 14, 2021
// Toolchain version: esp-2020r3-8.4.0/xtensa-esp32-elf
// Lvgl version:  v7.11.0 - commit: ec9de51, March, 2021
// Smooth version: master - commit: 5578b8b, April 15, 2021
// Bin file size: 1,524,272 bytes
//******************************************************************************************************************
#include "App.h"
#include <smooth/core/task_priorities.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/SystemStatistics.h>

using namespace smooth::core;
using namespace std::chrono;
using namespace smooth::core::io::spi;
using namespace smooth::core::filesystem;
using namespace smooth::application::sensor;

namespace redstone
{
    // Class Constants
    static const char* TAG = "APP";

    // Constructor
    App::App() : Application(APPLICATION_BASE_PRIO, seconds(60)), wifi(*this), sntp_task(*this)
    {
    }

    // Initialize the application
    void App::init()
    {
        Log::warning(TAG, "============ Starting APP  ===========");
        Application::init();
        lvgl_task.start();

        // allow lvgl task time to initialize the spi bus
        std::this_thread::sleep_for(seconds{ 2 });

        // initialize sdcard
        data_store.init();

        // allow sdcard to be initialized
        std::this_thread::sleep_for(seconds{ 3 });
        wifi.set_wifi_cred(data_store.read_nth_line("wifi_cred.txt", 0), data_store.read_nth_line("wifi_cred.txt", 1));
        wifi.start_wifi();
        poll_sensor_task.start();
        sntp_task.start();

        // allow time for wifi to connect
        std::this_thread::sleep_for(seconds{ 10 });
        wifi.show_network_info();
        wifi.show_local_mac_address();
        wifi.show_wifi_information();
    }

    // Tick event happens every 60 seconds
    void App::tick()
    {
        Log::warning(TAG, "============ M5StackBME280 TICK  =============");

        if (!heap_caps_check_integrity_all(true))
        {
            Log::error(TAG, "========= Heap Corrupted  ===========");
        }

        SystemStatistics::instance().dump();
    }
}
