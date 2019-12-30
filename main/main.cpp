/*
 * Main.cpp is used to call the external "C" inplementation of app_main 
 * used by esp-idf
 * 
 */

#include "App.h"
#include <iostream>

extern "C" {
	void app_main()
	{
		redstone::App app;
		app.start();
	}
}

