# The source files required to build project
set(SOURCES 
        main.cpp
        App.cpp
        App.h

        gui/LvglTask.cpp
        gui/LvglTask.h

        gui/Bme280View.cpp
        gui/Bme280View.h

        gui/LvDisplayDriver.cpp
        gui/LvDisplayDriver.h

        gui/GuiButton.cpp
        gui/GuiButton.h
        gui/GuiButtonPrev.cpp
        gui/GuiButtonPrev.h
        gui/GuiButtonNext.cpp
        gui/GuiButtonNext.h
        gui/HwPushButton.h

        gui/TitleBar.cpp
        gui/TitleBar.h

        gui/MenuBar.cpp
        gui/MenuBar.h

        gui/IContainer.h
        gui/ContentPane001.cpp
        gui/ContentPane001.h
        gui/ContentPane002.cpp
        gui/ContentPane002.h
        gui/ContentPane003.cpp
        gui/ContentPane003.h
        gui/ContentPane004.cpp
        gui/ContentPane004.h

        sntp/WifiSsidPasswd.h
        sntp/SntpTask.cpp
        sntp/SntpTask.h
        sntp/DateTimeValue.h
        
        model/ScanSensorTask.cpp
        model/ScanSensorTask.h
        model/Bme280Measurements.h
        )

