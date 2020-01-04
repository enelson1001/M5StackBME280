/****************************************************************************************
 * ContentPane002.cpp - A content pane that displays a temperature gauge and
 *                      humidity and pressure values
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
#include "gui/ContentPane002.h"

#include <smooth/core/logging/log.h>
using namespace smooth::core::logging;

namespace redstone
{
    // Class constants
    static const char* TAG = "ContentPane002";

    // Constructor
    ContentPane002::ContentPane002(smooth::core::Task& task_lvgl) :
            subr_queue_bme280_measurements(SubQBme280Measurements::create(2, task_lvgl, *this))

            // Create Subscriber Queue (SubQ) so this view can listen for
            // Bme280Measurements events
            // the queue will hold up to 2 items
            // the "task_lvgl" is this task which to signal when an event is available.
            // the "*this" is the class instance that will receive the events
    {
    }

    // Create the content pane for this view
    void ContentPane002::create(int width, int height)
    {
        Log::info(TAG, "Creating ContentPane002");

        // create style for the content container
        lv_style_copy(&content_container_style, &lv_style_plain);
        content_container_style.body.main_color = lv_color_hex3(0xaaa);
        content_container_style.body.grad_color = lv_color_hex3(0xaaa);

        // create style for temperature value
        lv_style_copy(&temperature_style, &lv_style_pretty);
        temperature_style.text.font = &lv_font_roboto_28;
        temperature_style.text.color = lv_color_hex3(0xC00);

        // create style for humidity and pressure values
        lv_style_copy(&humd_pres_style, &lv_style_pretty);
        humd_pres_style.text.font = &lv_font_roboto_16;
        humd_pres_style.text.color = lv_color_hex3(0x030);

        // create a content container
        content_container = lv_cont_create(lv_scr_act(), NULL);
        lv_obj_set_size(content_container, width, height);
        lv_obj_align(content_container, NULL, LV_ALIGN_CENTER, 0, 0);
        lv_cont_set_style(content_container, LV_CONT_STYLE_MAIN, &content_container_style);
        lv_obj_set_hidden(content_container, true);

        // create style for the temperature gauge
        lv_style_copy(&gauge_style, &lv_style_pretty);
        gauge_style.body.main_color = LV_COLOR_BLACK;       // Line color at the beginning
        gauge_style.body.grad_color = LV_COLOR_BLACK;       // Line color at the end*
        gauge_style.body.padding.left = 10;                 // Scale line length
        gauge_style.body.padding.inner = 8;                 // Scale label padding
        gauge_style.body.border.color = LV_COLOR_GREEN;     // Needle middle circle color
        gauge_style.line.width = 3;
        gauge_style.text.font = &lv_font_roboto_16;         // Gauge text font
        gauge_style.text.color = LV_COLOR_WHITE;            // Guage text color
        gauge_style.line.color = LV_COLOR_BLACK;            // Line color

        // set the color of the needle
        needle_colors[0] = lv_color_hex3(0xC00);

        // create gauge
        temperature_gauge = lv_gauge_create(content_container, NULL);
        lv_gauge_set_style(temperature_gauge, LV_GAUGE_STYLE_MAIN, &gauge_style);
        lv_gauge_set_needle_count(temperature_gauge, 1, needle_colors);
        lv_gauge_set_range(temperature_gauge, 32, 110);
        lv_obj_set_size(temperature_gauge, 150, 150);
        lv_obj_align(temperature_gauge, NULL, LV_ALIGN_CENTER, 0, 10);

        // create a dynamic label for temperature measurement value
        temperature_value_label = lv_label_create(content_container, NULL);
        lv_obj_set_style(temperature_value_label, &temperature_style);
        lv_label_set_text(temperature_value_label, "");

        // create a dynamic label for humidity measurement value
        humidity_value_label = lv_label_create(content_container, NULL);
        lv_obj_set_style(humidity_value_label, &humd_pres_style);
        lv_label_set_text(humidity_value_label, "");

        // create a dynamic label for pressure measurement value
        pressure_value_label = lv_label_create(content_container, NULL);
        lv_obj_set_style(pressure_value_label, &humd_pres_style);
        lv_label_set_text(pressure_value_label, "");
    }

    // The published event from Bme280Measurements
    void ContentPane002::event(const Bme280Measurements& event)
    {
        temperature = event.get_temperture_degree_F();
        humidty = event.get_relative_humidity();
        sea_level_pressure_inHg = event.get_sea_level_pressure_inHg();
        update_temperature_humidity_pressure();
    }

    // Update the measurement value labels
    void ContentPane002::update_temperature_humidity_pressure()
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
    void ContentPane002::show()
    {
        lv_obj_set_hidden(content_container, false);
    }

    // Hide the content pane
    void ContentPane002::hide()
    {
        lv_obj_set_hidden(content_container, true);
    }
}
