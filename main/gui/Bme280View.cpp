/****************************************************************************************
 * Bme280View.cpp - The BME280 views
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
#include "gui/Bme280View.h"
#include "gui/GuiButtonPrev.h"
#include "gui/GuiButtonNext.h"
#include "gui/HwPushButton.h"
#include "gui/TitleBar.h"
#include "gui/ContentPane001.h"
#include "gui/ContentPane002.h"
#include "gui/ContentPane003.h"
#include "gui/ContentPane004.h"

#include <smooth/core/logging/log.h>

using namespace smooth::core::logging;

namespace redstone
{
    // Class constants
    static const char* TAG = "Bme280View";

    // Constructor
    Bme280View::Bme280View(smooth::core::Task& task_lvgl) : task_lvgl(task_lvgl)
    {
    }

    // Initialize view controller
    void Bme280View::init()
    {
        Log::info(TAG, "====== Initializing Bme280View ======");

        // initialize the display driver
        lv_display_driver.initialize();

        // create the title bars
        title_bar = std::make_unique<TitleBar>("BME280 Measurements");
        title_bar->create(LV_HOR_RES, 40);
        title_bars[Measurements] = std::move(title_bar);

        title_bar = std::make_unique<TitleBar>("BME280 Temp Gauge");
        title_bar->create(LV_HOR_RES, 40);
        title_bars[TemperatureGauge] = std::move(title_bar);

        title_bar = std::make_unique<TitleBar>("BME280 Hourly Temp Log");
        title_bar->create(LV_HOR_RES, 40);
        title_bars[HourlyLog] = std::move(title_bar);

        title_bar = std::make_unique<TitleBar>("Time and Date");
        title_bar->create(LV_HOR_RES, 40);
        title_bars[TimeAndDate] = std::move(title_bar);

        // create content panes
        content_pane = std::make_unique<ContentPane001>(task_lvgl);
        content_pane->create(LV_HOR_RES, 158);
        content_panes[Measurements] = std::move(content_pane);

        content_pane = std::make_unique<ContentPane002>(task_lvgl);
        content_pane->create(LV_HOR_RES, 158);
        content_panes[TemperatureGauge] = std::move(content_pane);

        content_pane = std::make_unique<ContentPane003>(task_lvgl);
        content_pane->create(LV_HOR_RES, 158);
        content_panes[HourlyLog] = std::move(content_pane);

        content_pane = std::make_unique<ContentPane004>(task_lvgl);
        content_pane->create(LV_HOR_RES, 158);
        content_panes[TimeAndDate] = std::move(content_pane);

        // create menu bar
        menu_bar.initialize();
        menu_bar.add_menu_button(MenuBar::LeftButtonID, std::make_unique<GuiButtonPrev>(*this), std::make_unique<HwPushButton>(GPIO_NUM_39, false, false));
        menu_bar.add_menu_button(MenuBar::RightButtonID, std::make_unique<GuiButtonNext>(*this), std::make_unique<HwPushButton>(GPIO_NUM_37, false, false));
        menu_bar.create(LV_HOR_RES, 40);
        menu_bar.show();  // only need to do this once since we never change the menu bar

        // show new view
        show_new_view();
    }

    void Bme280View::show_new_view()
    {
        title_bars[new_view_id]->show();
        content_panes[new_view_id]->show();
        current_view_id = new_view_id;
    }

    void Bme280View::hide_current_view()
    {
        title_bars[current_view_id]->hide();
        content_panes[current_view_id]->hide();
    }

    void Bme280View::show_next_view()
    {
        hide_current_view();
        new_view_id = current_view_id == TimeAndDate ? Measurements : static_cast<ViewID>(static_cast<int>(current_view_id) + 1);
        show_new_view();
    }

    void Bme280View::show_previous_view()
    {
        hide_current_view();
        new_view_id = current_view_id == Measurements ? TimeAndDate : static_cast<ViewID>(static_cast<int>(current_view_id) - 1);
        show_new_view();
    }
}
