#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_sdlrenderer.h"

#ifdef WIN32
#include <windows.h>
#include <string>
#elif
//cry
#endif

#include "Display.h"
#include "Config.h"

std::string openFileDialog();

class GUI
{
public:
	GUI();

	void setDisplay(Display* display);

	void render();
	
	bool show = true;
	bool displayAbout = false;

	Display* display = nullptr;
};

