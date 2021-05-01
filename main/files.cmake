# The source files required to build project
set(SOURCES 
        main.cpp
        App.cpp
        App.h

        gui/LvglTask.cpp
        gui/LvglTask.h

        gui/ViewController.cpp
        gui/ViewController.h

        gui/DisplayDriver.cpp
        gui/DisplayDriver.h

        gui/GuiButton.cpp
        gui/GuiButton.h
        gui/GuiButtonPrev.cpp
        gui/GuiButtonPrev.h
        gui/GuiButtonNext.cpp
        gui/GuiButtonNext.h
        gui/HwPushButton.h

        gui/TitlePane.cpp
        gui/TitlePane.h

        gui/MenuPane.cpp
        gui/MenuPane.h

        gui/IPane.h
        gui/CPMeasurements.cpp
        gui/CPMeasurements.h
        gui/CPTempGauge.cpp
        gui/CPTempGauge.h
        gui/CPHourlyLog.cpp
        gui/CPHourlyLog.h
        gui/CPTimeAndDate.cpp
        gui/CPTimeAndDate.h

        sntp/SntpTask.cpp
        sntp/SntpTask.h
        sntp/TimeValue.h
        
        model/PollSensorTask.cpp
        model/PollSensorTask.h
        model/EnvirValue.h

        storage/DataStore.cpp
        storage/DataStore.h

        network/Wifi.cpp
        network/Wifi.h
        )

