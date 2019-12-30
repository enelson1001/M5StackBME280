/****************************************************************************************
 * Bme280Measurements.h - This class instance is published and received by subscribers
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

#include <string>
#include <math.h>

namespace redstone
{
    class Bme280Measurements
    {
        public:
            Bme280Measurements() {}

            /// Get the temperature in degree fahrenheit
            /// \param return Return the temperature in degree farenheit
            float get_temperture_degree_F() const
            {
                return ((temperature * 9) / 5) + 32;
            }

            /// Get the temperature in degree celsius
            /// \param return Return the temperature in degree celsius
            float get_temperature_degree_C()
            {
                return temperature;
            }

            /// Set the temperature in degree celsius
            /// \param value The temperature in degree celsius
            void set_temperture_degree_C(float value)
            {
                temperature = value;
            }

            /// Get the relative humidity
            /// \param return Return the realtive humidity
            float get_relative_humidity() const
            {
                return humidity;
            }

            /// Set the relative humidity
            /// \param value The relative humidity
            void set_relative_humidity(float value)
            {
                humidity = value;
            }

            /// Get the hPa pressure
            /// \param return Return the hPa pressure
            float get_pressure_hPa() const
            {
                return pressure;
            }

            /// Set the hPa pressure
            /// \param value The pressure in hPa
            void set_pressure_hPa(float value)
            {
                pressure = value;
            }

            /// Get the sea level pressure in hPa.
            /// Note: 794 in equation is the altitude of my current location in meters
            /// \param return Return the sea level pressure in hPa
            float get_sea_level_pressure_hPa() const
            {
                return pressure / pow(1 - ((0.0065 * 802) / (temperature + (0.0065 * 802) + 273.15)), 5.257);
            }

            /// Get the sea level pressure in inHg
            /// \param return Return the sea level pressure in inHg
            float get_sea_level_pressure_inHg() const
            {
                return get_sea_level_pressure_hPa() / 3386.389;
            }

        private:
            float temperature;
            float humidity;
            float pressure;
    };
}
