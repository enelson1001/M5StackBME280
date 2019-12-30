/****************************************************************************************
 * ContentPane004.cpp - A content pane that displays time and date
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
#include <smooth/core/logging/log.h>
#include "gui/ContentPane004.h"

//using namespace smooth::core;
using namespace smooth::core::logging;

namespace redstone
{
    // Class constants
    static const char* TAG = "ContentPane004";

    // Constructor
    ContentPane004::ContentPane004(smooth::core::Task& task_lvgl) :
            subr_queue_date_time_value(SubQDateTimeValue::create(2, task_lvgl, *this))

            // Create Subscriber Queue (SubQ) so this content pane
            // can listen for DateTime events
            // the queue will hold up to 2 items
            // the "task_lvgl" is this task which to signal when an event is available.
            // the "*this" is the class instance that will receive the events
    {
    }

    // Create the content pane for this view
    void ContentPane004::create(int width, int height)
    {
        Log::info(TAG, "Creating ContentPane004");

        // create style for the content container
        lv_style_copy(&content_container_style, &lv_style_plain);
        content_container_style.body.main_color = lv_color_hex3(0xaaa);
        content_container_style.body.grad_color = lv_color_hex3(0xaaa);
        content_container_style.text.font = &lv_font_roboto_28;
        content_container_style.text.color = LV_COLOR_NAVY;

        // create a content container
        content_container = lv_cont_create(lv_scr_act(), NULL);
        lv_obj_set_size(content_container, width, height);
        lv_obj_align(content_container, NULL, LV_ALIGN_CENTER, 0, 0);
        lv_cont_set_style(content_container, LV_CONT_STYLE_MAIN, &content_container_style);
        lv_obj_set_hidden(content_container, true);

        // create a dynamic label for time value
        time_value_label = lv_label_create(content_container, NULL);
        lv_obj_set_style(time_value_label, &content_container_style);
        lv_label_set_text(time_value_label, "");

        // create a dynamic label for date value
        date_value_label = lv_label_create(content_container, NULL);
        lv_obj_set_style(date_value_label, &content_container_style);
        lv_label_set_text(date_value_label, "");
    }

    // The published event from DateTimeValue
    void ContentPane004::event(const DateTimeValue& event)
    {
        time = event.get_time();
        date = event.get_date();
        update_time_and_date();
    }

    // Update the time and date texts
    void ContentPane004::update_time_and_date()
    {
        // Update time value
        lv_label_set_text(time_value_label, time.c_str());
        lv_obj_align(time_value_label, NULL, LV_ALIGN_CENTER, 0, -20);

        // Update date value
        lv_label_set_text(date_value_label, date.c_str());
        lv_obj_align(date_value_label, time_value_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    }

    // Show the content pane
    void ContentPane004::show()
    {
        lv_obj_set_hidden(content_container, false);
    }

    // Hide the content pane
    void ContentPane004::hide()
    {
        lv_obj_set_hidden(content_container, true);
    }
}