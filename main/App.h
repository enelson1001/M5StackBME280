/****************************************************************************************
 * App.h - The Application class
<<<<<<< HEAD
 * 
=======
>>>>>>> cdaf9695e0182962cfecd6d15f73a70dcc143e78
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
<<<<<<< HEAD
#include "model/PollSensorTask.h"
=======
#include "model/ScanSensorTask.h"
>>>>>>> cdaf9695e0182962cfecd6d15f73a70dcc143e78

namespace redstone
{
    class App : public smooth::core::Application
    {
        public:
            App();

            void init() override;

            void tick() override;

        private:
            LvglTask lvgl_task{};
<<<<<<< HEAD
            PollSensorTask poll_sensor_task{};
=======
            ScanSensorTask scan_sensor_task{};
>>>>>>> cdaf9695e0182962cfecd6d15f73a70dcc143e78
            SntpTask sntp_task;
    };
}
