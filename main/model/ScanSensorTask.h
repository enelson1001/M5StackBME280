/****************************************************************************************
 * ScanSensorTask.h - A task that runs periodically to retreive BME280 measurements.
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
#pragma once

#include "Bme280Measurements.h"
#include <smooth/core/Task.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/ipc/SubscribingTaskEventQueue.h>
#include <smooth/core/io/i2c/Master.h>
#include <smooth/application/io/i2c/BME280.h>

namespace redstone
{
    class ScanSensorTask : public smooth::core::Task
    {
        public:
            ScanSensorTask();

            void init() override;

            void tick() override;

        private:
            bool init_i2c_bme280();
            
            smooth::core::io::i2c::Master i2c_master;
            std::unique_ptr<smooth::application::sensor::BME280> thp_sensor{};
            bool bme280_initialized{ false };
            Bme280Measurements bme280_measurements {};
    };
}
