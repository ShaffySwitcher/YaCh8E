#pragma once

#include <SDL.h>
#include "Chip8.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_sdlrenderer.h"

class Display
{
public:
	Display();
	~Display();

	bool init();
	void close();

	void setChip8(Chip8* chip8) { this->chip8 = chip8; } //Is there a better way?

	SDL_Window* getWindow() { return window; }
	SDL_Renderer* getRenderer() { return renderer; }
	SDL_Texture* getTexture() { return gameTexture; }

	Chip8* getChip8() { return chip8; }

	void update(); //Probably should be called render(), I dunno...
private:
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Surface* gameSurface = nullptr; // This is my first ever SDL project...
	SDL_Texture* gameTexture = nullptr;

	Chip8* chip8 = nullptr;
};

