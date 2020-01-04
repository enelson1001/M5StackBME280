/****************************************************************************************
 * Bme280View.h - The BME280 view
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

#include <memory>                   // for unique_ptr
#include <unordered_map>
#include <smooth/core/Task.h>
#include "gui/LvDisplayDriver.h"
#include "gui/MenuBar.h"
#include "gui/IContainer.h"

namespace redstone
{
    class Bme280View
    {
        public:
            // Constants & Enums
            enum ViewID : int
            {
                Measurements = 0,
                TemperatureGauge,
                HourlyLog,
                TimeAndDate
            };

            // Constructor
            Bme280View(smooth::core::Task& task_lvgl);

            /// Initialize the bme280 view
            void init();

            /// Show the new view
            void show_new_view();

            /// Hide the current view
            void hide_current_view();

            /// Show the next view
            void show_next_view();

            /// Show the previous view
            void show_previous_view();

        private:
            smooth::core::Task& task_lvgl;
            LvDisplayDriver lv_display_driver;

            std::unique_ptr<IContainer> content_pane;
            std::unique_ptr<IContainer> title_bar;
            MenuBar menu_bar{};

            std::unordered_map<ViewID, std::unique_ptr<IContainer>> content_panes;
            std::unordered_map<ViewID, std::unique_ptr<IContainer>> title_bars;
            ViewID current_view_id{ HourlyLog };
            ViewID new_view_id{ HourlyLog };
    };
}
