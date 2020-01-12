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
// Typical output on M5Stack (after running for 1 hour; timestamp = 3606531)
//
//  MemStat: Mem type |  8-bit free | Smallest block | Minimum free | 32-bit free | Smallest block | Minimum free
//  MemStat: INTERNAL |      107228 |         106100 |        78360 |      158072 |         106100 |       129196
//  MemStat:      DMA |      107228 |         106100 |        78360 |      107228 |         106100 |        78360
//  MemStat:   SPIRAM |           0 |              0 |            0 |           0 |              0 |            0
//  MemStat:
//  MemStat:             Name |      Stack |  Min free stack |  Max used stack
//  MemStat:         LvglTask |       4096 |             456 |            3640
//  MemStat:         SntpTask |       3200 |             768 |            2432
//  MemStat:   PollSensorTask |       3300 |             480 |            2820
//  MemStat: SocketDispatcher |      20480 |           18228 |            2252
//  MemStat:         MainTask |      16384 |           12444 |            3940
//
// Esp32-IDF version: v4.0-beta2
// Toolchain version: xtensa-esp32-elf-gcc (crosstool-NG esp32-2019r1) 8.2.0
// Lvgl version: v6.1.1-39-g7d25f66
// Smooth version: master SHA1: 24a828e1
// Bin file size: 1,246,704 bytes
//******************************************************************************************************************
#include "App.h"
#include <smooth/core/task_priorities.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/SystemStatistics.h>

using namespace smooth::core;
using namespace std::chrono;

namespace redstone
{
    // Class Constants
    static const char* TAG = "APP";

    // Constructor
    App::App() : Application(APPLICATION_BASE_PRIO, seconds(60)), sntp_task(*this)
    {
    }

    // Initialize the application
    void App::init()
    {
        Log::warning(TAG, "============ Starting APP  ===========");
        Application::init();
        sntp_task.start();
        lvgl_task.start();
        poll_sensor_task.start();
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
