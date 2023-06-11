#pragma once

#include <cstdint>
#include <stack>
#include <array>
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <SDL_stdinc.h>
#include <SDL_audio.h>

#include "Memory.h"
#include "Random.h"
#include "Config.h"

#define SCREEN_X 64
#define SCREEN_Y 32

#define RAM_SIZE 4096

struct ROM_Info {
	std::string path;
	size_t size;
};

struct Chip8Quirks {
	bool logicQuirks;
	bool loadStoreQuirks;
	bool vBlankQuirks;
	bool clipQuirks;
	bool shiftQuirks;
	bool jumpQuirks;
};

class Chip8
{
public:
	Chip8();
	~Chip8();

	void loadProgram(std::string path);
	void reset();
	void updateTimers();
	void update();

	void setActive(bool active) { this->running = active; }
	void setPaused(bool paused) { this->paused = paused; }
	void setFrameAdvance(bool frmadv) { this->frameAdvance = frmadv; }
	void setKey(uint8_t key, bool pressed) { keys[key] = pressed; }
	void setDisplayRefreshRate(uint32_t speed) { this->displayRefreshRate = speed; }
	void setRefreshRate(uint32_t speed) { this->refreshRate = speed; }
	void setQuirks(Chip8Quirks quirks);

	uint8_t* getVram() { return vram.data(); }
	uint32_t getDisplayRefreshRate() { return displayRefreshRate; } // the CHIP-8's of course...
	uint32_t getRefreshRate() { return refreshRate; } // the CHIP-8's of course...
	ROM_Info getRomInfo() { return romInfo; }
	bool getActive() { return running; }
	bool getPaused() { return paused; }
	bool getFrameAdvance() { return frameAdvance; }
	Chip8Quirks getQuirks();

	bool drawFlag = false;
private:
	std::array<uint8_t, RAM_SIZE> ram;
	std::array<uint8_t, SCREEN_X * SCREEN_Y> vram;
	
	uint16_t pc;
	uint16_t i;
	std::stack<uint16_t> stack;
	uint8_t delayTimer;
	uint8_t soundTimer;

	bool waitingKey = false;
	std::array<bool, 16> keys;

	uint8_t registers[16];

	uint32_t displayRefreshRate; // in Hertz
	uint32_t refreshRate; // in Hertz

	bool running = false;
	bool paused = false;
	bool frameAdvance = false; // there should be a better way to do this...

	bool logicQuirks = false;
	bool loadStoreQuirks = false;
	bool vBlankQuirks = false;
	bool clipQuirks = false;
	bool shiftQuirks = false;
	bool jumpQuirks = false;

	//technically unrelated to the emulator... aka this is messy and needs to move

	ROM_Info romInfo;
	
	void loadFont();
	void clearScreen();
};

inline void audioCallback(void* userdata, Uint8* stream, int len) {
	int numSamples = len / 2;
	Sint16* audioBuffer = reinterpret_cast<Sint16*>(stream);

	const double period = static_cast<double>(48000) / 440.0;
	const int samplesPerHalfPeriod = static_cast<int>(period) / 2;

	for (int i = 0; i < numSamples; ++i) {
		int sampleIndex = i % samplesPerHalfPeriod;
		audioBuffer[i] = (sampleIndex < samplesPerHalfPeriod / 2) ? 32767 : -32768;
	}
}