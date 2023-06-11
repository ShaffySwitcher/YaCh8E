#pragma once

#include <SDL.h>
#include <sstream>
#include <iomanip>

#include "ini.h"

// TO-DO: generate new template when file doesn't exist

class Config {
public:
	void loadConfig();
	void saveConfig();

	uint8_t keyboardLayout[16];

	uint8_t system; //System to emulate (0: chip-8)
	int baseSpeed, baseDrawSpeed;

	SDL_Color fillColor, backgroundColor;

	int frameSize;
	bool muteAudio;
	bool fullscreen;
};

extern Config CONFIG;