/****************************************************************************************
 * ContentPane001.h - A content pane that displays temperature humidity and pressure values
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
#include "model/Bme280Measurements.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
#include <lvgl/lvgl.h>
#pragma GCC diagnostic pop

namespace redstone
{
    class ContentPane001 : public IContainer, public smooth::core::ipc::IEventListener<Bme280Measurements>
    {
        public:
            /// Constructor
            /// \param task_lvgl The task this class is running under
            ContentPane001(smooth::core::Task& task_lvgl);

            /// Show the content pane
            void show() override;

            /// Hide the content pane
            void hide() override;

            /// Create the content pane
            /// \param width The width of the content pane
            /// \param height The height of the content pane
            void create(int width, int height) override;

            /// The Bme280Measurements event that this instance listens for
            void event(const Bme280Measurements& event) override;

        private:
            /// Update the measurements text (ie temperature, humidity, pressure)
            void update_measurements_text();

            // Subscriber's queue's
            using SubQBme280Measurements = smooth::core::ipc::SubscribingTaskEventQueue<Bme280Measurements>;
            std::shared_ptr<SubQBme280Measurements> subr_queue_bme280_measurements;

            lv_style_t content_container_style;
            lv_style_t measurement_container_style;
            lv_style_t descriptor_label_style;
            lv_style_t measurement_label_style;

            lv_obj_t* content_container;
            lv_obj_t* temperature_container;
            lv_obj_t* humdity_container;
            lv_obj_t* pressure_container;
            lv_obj_t* temperature_value_label;
            lv_obj_t* humidity_value_label;
            lv_obj_t* pressure_value_label;

            float temperature;
            float humidty;
            float sea_level_pressure_inHg;
    };
}
