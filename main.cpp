#include <SDL.h>
#include <SDL_audio.h>
#include <iostream>
#include "Display.h"
#include "Config.h"
#include "GUI.h"

/*
* TODO:
* 
* - Debug UI (imGui)
* - Optimization
* - get rid of magic numbers funny
* 
*/

extern bool quit;

bool init(Display& display) {
	bool success = true;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << '\n';
		success = false;
	}
	else
	{
		if (display.init() != true) {
			std::cout << "Display could not initialize! SDL Error: " << SDL_GetError() << '\n';
			success = false;
		}
		else {
			SDL_AudioSpec audioSpec{};
			audioSpec.freq = 48000;
			audioSpec.format = AUDIO_S16SYS;
			audioSpec.channels = 1;
			audioSpec.samples = 2048;
			audioSpec.callback = audioCallback;

			if (SDL_OpenAudio(&audioSpec, nullptr) < 0) {
				std::cout << "Sound could not initialize! SDL Error: " << SDL_GetError() << '\n';
				success = false;
			}
		}
	}

	return success;
}

int main(int argc, char* args[])
{
	CONFIG.loadConfig();

	Display display;
	GUI gui;

	if (!init(display))
	{
		std::cout << "Failed to initialize!\n";
	}
	else
	{
		Chip8 chip8;
		display.setChip8(&chip8);
		gui.setDisplay(&display);

		SDL_Event e;

		uint32_t previousDrawTime = 0;
		uint32_t previousTickTime = 0;
		uint32_t previous60HzTime = 0;

		while (!quit)
		{
			while (SDL_PollEvent(&e) != 0)
			{
				ImGui_ImplSDL2_ProcessEvent(&e);
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
				if (e.type == SDL_KEYDOWN) {
					if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
						quit = true;
					}
					if (e.key.keysym.scancode == SDL_SCANCODE_SPACE) {
						gui.show = !gui.show;
					}
					for (int i = 0; i < 16; ++i) {
						if (e.key.keysym.scancode == CONFIG.keyboardLayout[i]) {
							chip8.setKey(i, true);
						}
					}
				}
				if (e.type == SDL_KEYUP) {
					for (int i = 0; i < 16; ++i) {
						if (e.key.keysym.scancode == CONFIG.keyboardLayout[i]) {
							chip8.setKey(i, false);
						}
					}
				}
			}

			uint32_t currentTicks = SDL_GetTicks();

			Uint32 elapsedTickTime = currentTicks - previousTickTime;
			if (elapsedTickTime >= (1.0f / chip8.getRefreshRate()) * 1000) {
				chip8.update();
				previousTickTime = currentTicks;
			}

			// RENDER

			SDL_SetRenderDrawColor(display.getRenderer(), CONFIG.backgroundColor.r, CONFIG.backgroundColor.g, CONFIG.backgroundColor.b, 255);
			SDL_RenderClear(display.getRenderer());

			Uint32 elapsedDrawTime = currentTicks - previousDrawTime;
			if (elapsedDrawTime >= (1.0f / chip8.getDisplayRefreshRate()) * 1000) {
				chip8.updateTimers();
				display.update();
				previousDrawTime = currentTicks;
			}

			Uint32 elapsed60HzTime = currentTicks - previous60HzTime;
			if (elapsed60HzTime >= (1.0f / 60) * 1000) {
				gui.render();

				SDL_RenderCopy(display.getRenderer(), display.getTexture(), nullptr, nullptr);

				ImGui::Render();
				ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
				SDL_RenderPresent(display.getRenderer());

				previous60HzTime = currentTicks;
			}
		}
	}

	display.close();
	SDL_CloseAudio();
	SDL_Quit();

	CONFIG.saveConfig();

	return 0;
}