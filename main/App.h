/****************************************************************************************
 * App.h - The Application class
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
#pragma once

#include <smooth/core/Application.h>
#include "sntp/SntpTask.h"
#include "gui/LvglTask.h"
#include "model/PollSensorTask.h"
#include "storage/DataStore.h"
#include "network/Wifi.h"

namespace redstone
{
    class App : public smooth::core::Application
    {
        public:
            App();

            void init() override;

            void tick() override;

        private:
            DataStore data_store;
            Wifi wifi;
            LvglTask lvgl_task{};
            PollSensorTask poll_sensor_task{};
            SntpTask sntp_task;
    };
}
