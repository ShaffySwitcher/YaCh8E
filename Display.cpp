#include "Display.h"

Config CONFIG;

Display::Display()
{
}

Display::~Display()
{
}

bool Display::init()
{
	bool success = true;

	int windowXSize = CONFIG.frameSize * 64;
	int windowYSize = CONFIG.frameSize * 32;

	window = SDL_CreateWindow("YaCh8E", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowXSize, windowYSize, SDL_WINDOW_SHOWN);
	if (window == nullptr)
	{
		std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << '\n';
		success = false;
	}
	else {
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if (renderer == nullptr) {
			std::cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << '\n';
			success = false;
		}
		else {
			SDL_RenderSetLogicalSize(renderer, windowXSize, windowYSize);
			SDL_RenderSetIntegerScale(renderer, SDL_TRUE);
			SDL_SetRenderDrawColor(renderer, 64, 0, 0, 255);
			if (CONFIG.fullscreen) {
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
			}

			gameTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 64, 32);
		}
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);

	return success;
}

void Display::close()
{
	SDL_DestroyTexture(gameTexture);
	gameTexture = nullptr;

	SDL_DestroyRenderer(renderer);
	renderer = nullptr;

	SDL_DestroyWindow(window);
	window = nullptr;
}

void Display::update()
{
	if (chip8) {
		if (chip8->drawFlag) {
			SDL_SetRenderTarget(renderer, gameTexture); // Set the render target to the texture

			SDL_SetRenderDrawColor(renderer, CONFIG.backgroundColor.r, CONFIG.backgroundColor.g, CONFIG.backgroundColor.b, 255); // Set the clear color (black in this case)
			SDL_RenderClear(renderer); // Clear the renderer

			SDL_SetRenderDrawColor(renderer, CONFIG.fillColor.r, CONFIG.fillColor.g, CONFIG.fillColor.b, 255);

			for (auto i = 0; i < SCREEN_Y; i++) {
				for (auto j = 0; j < SCREEN_X; j++) {
					if (chip8->getVram()[(i * SCREEN_X) + j]) {
						SDL_RenderDrawPoint(renderer, j, i);
					}
				}
			}

			SDL_SetRenderTarget(renderer, nullptr);

			chip8->drawFlag = false;

			if (chip8->getFrameAdvance()) {
				chip8->setPaused(true);
				chip8->setFrameAdvance(false);
			}
		}
	}
}
