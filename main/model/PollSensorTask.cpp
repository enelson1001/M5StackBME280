/****************************************************************************************
 * PollSensorTask.cpp - A task that runs periodically to poll the BME280 for measurements
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
#include "model/PollSensorTask.h"
#include <smooth/core/ipc/Publisher.h>

using namespace std::chrono;
using namespace smooth::core;
using namespace smooth::core::ipc;
using namespace smooth::application::sensor;

namespace redstone
{
    // Class constants
    static const char* TAG = "PollSensorTask";

    // Constructor
    PollSensorTask::PollSensorTask() :
            smooth::core::Task("PollSensorTask", 3300, 10, milliseconds(2000)),

            // The Task Name = "PollSensorTask"
            // The stack size is 3200 bytes
            // The priority is set to 10
            // The tick interval is 2 sec

            i2c_master(I2C_NUM_0,                       // I2C Port 0
                       GPIO_NUM_22,                     // SCL pin
                       false,                           // SCL internal pullup NOT enabled
                       GPIO_NUM_21,                     // SDA pin
                       false,                           // SDA internal pullup NOT enabled
                       100 * 1000)                      // clock frequency - 100kHz 

    {
    }

    // Initialize the Task
    void PollSensorTask::init()
    {
        bme280_initialized = init_i2c_bme280();
        Log::info(TAG, "BME280 intialization --- {}", bme280_initialized ? "Succeeded" : "Failed");
    }

    // Initialize the I@C BME280 device
    bool PollSensorTask::init_i2c_bme280()
    {
        bool res = false;
        auto device = i2c_master.create_device<BME280>(0x76);   // BME280 i2c address

        Log::info(TAG, "Scanning for BME280");

        if (device->is_present())
        {
            Log::info(TAG, "BME280 reset: {}", device->reset());

            bool measuring = false;
            bool loading_from_nvm = false;

            while (!device->read_status(measuring, loading_from_nvm) || loading_from_nvm)
            {
                Log::info(TAG, "Waiting for BME280 to complete reset operation... {} {}", measuring, loading_from_nvm);
            }

            res = device->configure_sensor(BME280Core::SensorMode::Normal,
                                           BME280Core::OverSampling::Oversamplingx1,
                                           BME280Core::OverSampling::Oversamplingx1,
                                           BME280Core::OverSampling::Oversamplingx1,
                                           BME280Core::StandbyTimeMS::ST_1000,
                                           BME280Core::FilterCoeff::FC_OFF);

            Log::info(name, "Configure BME280: {}", res);

            if (res)
            {
                Log::info(TAG, "BME280 initialized, ID: {}", device->read_id());
            }
            else
            {
                Log::error(name, "Could not init BME280");
            }
        }
        else
        {
            Log::error(TAG, "BME280 not present");
        }

        thp_sensor = std::move(device);

        return res;
    }


    // The task tick event happens every 2 seconds
    void PollSensorTask::tick()
    {
        if (bme280_initialized)
        {
            float temperature, humidity, pressure;
            thp_sensor->read_measurements(humidity, pressure, temperature);

            envir_value.set_temperture_degree_C(temperature);
            envir_value.set_relative_humidity(humidity);
            envir_value.set_pressure_hPa(pressure);
            
            Publisher<EnvirValue>::publish(envir_value);
        }
    }
}
