/****************************************************************************************
 * GuiButtonPrev.h - A class that creates a PREV gui button
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

#include "gui/GuiButton.h"
#include "gui/Bme280View.h"

namespace redstone
{
    class GuiButtonPrev : public GuiButton
    {
        public:
            /// Constructor
            /// \param view_renderer The class that will render the view
            GuiButtonPrev(Bme280View& view);

            /// On-Clicked event
            /// \param btn The button object that caused the on-clicked event
            virtual void on_clicked(lv_obj_t* btn) override;

            /// Create the NEXT button
            /// \param parent The parent for this gui button
            virtual lv_obj_t* create(lv_obj_t* parent) override;

        private:
            Bme280View& bme280_view;
            lv_style_t gui_btn_style;
    };
}
