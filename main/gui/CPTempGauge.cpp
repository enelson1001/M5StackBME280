/****************************************************************************************
 * CPTempGauge.cpp - A content pane that displays a temperature gauge and
 *                      humidity and pressure values
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
#include <sstream>
#include <iomanip>  // for set precision
#include "gui/CPTempGauge.h"

#include <smooth/core/logging/log.h>
using namespace smooth::core::logging;

namespace redstone
{
    // Class constants
    static const char* TAG = "CPTempGauge";

    // Constructor
    CPTempGauge::CPTempGauge(smooth::core::Task& task_lvgl) :
            subr_queue_envir_value(SubQEnvirValue::create(2, task_lvgl, *this))

            // Create Subscriber Queue (SubQ) so this view can listen for
            // EnvirValue events
            // the queue will hold up to 2 items
            // the "task_lvgl" is this task which to signal when an event is available.
            // the "*this" is the class instance that will receive the events
    {
    }

    // Create the content pane for this view
    void CPTempGauge::create(int width, int height)
    {
        Log::info(TAG, "Creating CPTempGauge");

        // create a plain style
        lv_style_init(&plain_style);
        lv_style_set_pad_top(&plain_style, LV_STATE_DEFAULT, 0);
        lv_style_set_pad_bottom(&plain_style, LV_STATE_DEFAULT, 0);
        lv_style_set_pad_left(&plain_style, LV_STATE_DEFAULT, 0);
        lv_style_set_pad_right(&plain_style, LV_STATE_DEFAULT, 0);
        lv_style_set_line_opa(&plain_style, LV_STATE_DEFAULT, 0);
        lv_style_set_pad_inner(&plain_style, LV_STATE_DEFAULT, 0);
        lv_style_set_margin_all(&plain_style, LV_STATE_DEFAULT, 0);
        lv_style_set_border_width(&plain_style, LV_STATE_DEFAULT, 0);
        lv_style_set_radius(&plain_style, LV_STATE_DEFAULT, 0);

        // create a style for the content container
        lv_style_copy(&content_container_style, &plain_style);
        lv_style_set_bg_color(&content_container_style, LV_STATE_DEFAULT, lv_color_hex3(0xaaa));

        // create style for temperature value
        lv_style_init(&temperature_style);
        lv_style_set_text_font(&temperature_style, LV_STATE_DEFAULT, &lv_font_montserrat_28);
        lv_style_set_text_color(&temperature_style, LV_STATE_DEFAULT, lv_color_hex3(0xC00));

        // create style for humidity and pressure values
        lv_style_init(&humd_pres_style);
        lv_style_set_text_font(&humd_pres_style, LV_STATE_DEFAULT, &lv_font_montserrat_16);
        lv_style_set_text_color(&humd_pres_style, LV_STATE_DEFAULT, lv_color_hex3(0x030));

        // create a content container
        content_container = lv_cont_create(lv_scr_act(), NULL);
        lv_obj_set_size(content_container, width, height);
        lv_obj_align(content_container, NULL, LV_ALIGN_CENTER, 0, 0);
        lv_obj_add_style(content_container, LV_CONT_PART_MAIN, &content_container_style);
        lv_obj_set_hidden(content_container, true);

        // create styles for the temperature gauge
        lv_style_init(&gauge_style);
        lv_style_set_bg_color(&gauge_style, LV_STATE_DEFAULT, lv_color_hex3(0xaaa));    // Gauge background color
        lv_style_set_border_color(&gauge_style, LV_STATE_DEFAULT, lv_color_hex3(0xaaa));// Gauge border color
        lv_style_set_pad_left(&gauge_style, LV_STATE_DEFAULT, 10);                      // Gauge outside padding
        lv_style_set_pad_inner(&gauge_style, LV_STATE_DEFAULT, 12);                     // Scale label padding
        lv_style_set_text_font(&gauge_style, LV_STATE_DEFAULT, &lv_font_montserrat_12); // Scale label font size
        lv_style_set_text_color(&gauge_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);        // Scale label color
        lv_style_set_line_color(&gauge_style, LV_STATE_DEFAULT, LV_COLOR_YELLOW);       // Scale minor line color
        lv_style_set_scale_end_color(&gauge_style, LV_STATE_DEFAULT, LV_COLOR_YELLOW);  // Scale minor end color
        lv_style_set_scale_grad_color(&gauge_style, LV_STATE_DEFAULT, LV_COLOR_YELLOW); // Scale minor grad color
        lv_style_set_line_width(&gauge_style, LV_STATE_DEFAULT, 2);                     // Scale minor line width
        lv_style_set_scale_end_line_width(&gauge_style, LV_STATE_DEFAULT, 2);           // Scale minor line length
        lv_style_set_scale_end_border_width(&gauge_style, LV_STATE_DEFAULT, 0);         // Scale minor - no end border

        lv_style_init(&needle_style);
        lv_style_set_line_width(&needle_style, LV_STATE_DEFAULT, 3);                    // Needle line width
        lv_style_set_bg_color(&needle_style, LV_STATE_DEFAULT, LV_COLOR_YELLOW);        // Needle center circle color

        lv_style_init(&major_style);
        lv_style_set_line_color(&major_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);        // Scale major line color
        lv_style_set_scale_end_color(&major_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);   // Scale major end color
        lv_style_set_scale_grad_color(&major_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);  // Scale major grad color
        lv_style_set_line_width(&major_style, LV_STATE_DEFAULT, 4);                     // Scale major line width
        lv_style_set_scale_end_line_width(&major_style, LV_STATE_DEFAULT, 4);           // Scale major line length
        lv_style_set_scale_end_border_width(&major_style, LV_STATE_DEFAULT, 0);         // Scale major - no end border

        // set the color of the needle
        needle_colors[0] = lv_color_hex3(0xC00);

        // create gauge
        temperature_gauge = lv_gauge_create(content_container, NULL);
        lv_obj_add_style(temperature_gauge, LV_GAUGE_PART_MAIN, &gauge_style);
        lv_obj_add_style(temperature_gauge, LV_GAUGE_PART_NEEDLE, &needle_style);
        lv_obj_add_style(temperature_gauge, LV_GAUGE_PART_MAJOR, &major_style);
        lv_gauge_set_needle_count(temperature_gauge, 1, needle_colors);
        lv_gauge_set_range(temperature_gauge, 32, 110);
        lv_obj_set_size(temperature_gauge, 150, 150);

        //lv_obj_align(temperature_gauge, NULL, LV_ALIGN_CENTER, 0, 10);
        lv_obj_align(temperature_gauge, NULL, LV_ALIGN_CENTER, 0, -10);

        // create a dynamic label for temperature measurement value
        temperature_value_label = lv_label_create(content_container, NULL);
        lv_obj_add_style(temperature_value_label, LV_LABEL_PART_MAIN, &temperature_style);
        lv_label_set_text(temperature_value_label, "");

        // create a dynamic label for humidity measurement value
        humidity_value_label = lv_label_create(content_container, NULL);
        lv_obj_add_style(humidity_value_label, LV_LABEL_PART_MAIN, &humd_pres_style);
        lv_label_set_text(humidity_value_label, "");

        // create a dynamic label for pressure measurement value
        pressure_value_label = lv_label_create(content_container, NULL);
        lv_obj_add_style(pressure_value_label, LV_LABEL_PART_MAIN, &humd_pres_style);
        lv_label_set_text(pressure_value_label, "");
    }

    // The published EnvirValue event
    void CPTempGauge::event(const EnvirValue& event)
    {
        temperature = event.get_temperture_degree_F();
        humidty = event.get_relative_humidity();
        sea_level_pressure_inHg = event.get_sea_level_pressure_inHg();
        update_temperature_humidity_pressure();
    }

    // Update the environmental value labels
    void CPTempGauge::update_temperature_humidity_pressure()
    {
        std::ostringstream stream;

        stream << std::fixed << std::setprecision(1) << temperature;
        std::string temp_text = stream.str() + " F";
        lv_label_set_text(temperature_value_label, temp_text.c_str());
        lv_obj_align(temperature_value_label, NULL, LV_ALIGN_CENTER, 0, 55);
        lv_gauge_set_value(temperature_gauge, 0, static_cast<int16_t>(temperature));

        stream.str("");
        stream.clear();
        stream << std::fixed << std::setprecision(0) << humidty;
        std::string humd_text = stream.str() + " %RH";
        lv_label_set_text(humidity_value_label, humd_text.c_str());
        lv_obj_align(humidity_value_label, NULL, LV_ALIGN_CENTER, -110, 60);

        stream.str("");
        stream.clear();
        stream << std::fixed << std::setprecision(2) << sea_level_pressure_inHg;
        std::string pres_text = stream.str() + " inHg";
        lv_label_set_text(pressure_value_label, pres_text.c_str());
        lv_obj_align(pressure_value_label, NULL, LV_ALIGN_CENTER, 110, 60);
    }

    // Show the content pane
    void CPTempGauge::show()
    {
        lv_obj_set_hidden(content_container, false);
    }

    // Hide the content pane
    void CPTempGauge::hide()
    {
        lv_obj_set_hidden(content_container, true);
    }
}
