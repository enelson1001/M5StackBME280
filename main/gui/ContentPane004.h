/****************************************************************************************
 * ContentPane004.h - A content pane that displays time and date
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

#include <memory>  // for shared pointer
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/ipc/SubscribingTaskEventQueue.h>
#include "gui/IContainer.h"
#include "sntp/DateTimeValue.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
#include <lvgl/lvgl.h>
#pragma GCC diagnostic pop

namespace redstone
{
    class ContentPane004 : public IContainer, public smooth::core::ipc::IEventListener<DateTimeValue>
    {
        public:
            /// Constructor
            /// \param task_lvgl The task this class is running under
            ContentPane004(smooth::core::Task& task_lvgl);

            /// Show the content pane
            void show() override;

            // Hide the content pane
            void hide() override;

            /// Create the content pane
            /// \param width The width of the content pane
            /// \param height The height of the content pane
            void create(int width, int height) override;

            /// The time event that this instance listens for
            void event(const DateTimeValue& ev) override;

        private:
            /// Update the time and date display values
            void update_time_and_date();

            // Subscriber's queue's
            using SubQDateTimeValue = smooth::core::ipc::SubscribingTaskEventQueue<DateTimeValue>;
            std::shared_ptr<SubQDateTimeValue> subr_queue_date_time_value;

            lv_style_t content_container_style;
            lv_obj_t* content_container;
            lv_obj_t* time_value_label;
            lv_obj_t* date_value_label;

            std::string time;
            std::string date;
    };
}
