/****************************************************************************************
 * TitleBar.h - A class that creates a simple title bar with a text heading
 * Created on Dec. 10, 2019
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

#include <string>
#include "gui/IContainer.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
#include <lvgl/lvgl.h>
#pragma GCC diagnostic pop

namespace redstone
{
    class TitleBar : public IContainer
    {
        public:
            TitleBar(std::string title);

            ~TitleBar() {}

            /// Create a title bar
            /// \param width The width of the title bar
            /// \param height The height of the title bar
            void create(int width, int height) override;

            //void set_title(std::string title);

            /// Show the title bar
            void show() override;

            /// Hide the title bar
            void hide() override;

        private:
            lv_style_t title_style;
            lv_obj_t* title_container;
            lv_obj_t* title_label;
            std::string title;
    };
}
