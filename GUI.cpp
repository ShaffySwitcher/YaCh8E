#include "GUI.h"

bool quit = false;

GUI::GUI()
{
}

void GUI::setDisplay(Display* display)
{
	this->display = display;
}

void GUI::render() {
	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	if (!display->getChip8()) {
		std::cout << "wtf??" << std::endl;
		return; //wtf?????
	}

	if (show) {
		if (displayAbout) {
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);

			if (ImGui::Begin("Example: Fullscreen window", &displayAbout, flags))
			{
				ImGui::Text("YaCh8E - A C++ Chip-8 Emulator in C++!\n(Yet another Chip-8 Emulator)\n(C) 2023 Shaffy, licensed under the GPL-3.0 license\n");
				ImGui::Text("TO-DO: Display versioning...");

				if (&displayAbout && ImGui::Button("Close")) {
					displayAbout = false;
				}
			}
			ImGui::End();
		}

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Load ROM")) {
					std::string romPath = openFileDialog();

					if (!romPath.empty()) {
						display->getChip8()->reset();
						display->getChip8()->loadProgram(romPath); //check if path is valid?
						display->getChip8()->setActive(true);
						show = false;
					}
				}
				ImGui::Separator();
				if (ImGui::MenuItem("About")) {
					displayAbout = !displayAbout;
				}
				if (ImGui::MenuItem("Exit")) {
					quit = true;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Emulation"))
			{
				if (ImGui::MenuItem("Boot", 0, false, !display->getChip8()->getActive())) {
					display->getChip8()->reset();
					display->getChip8()->setActive(true);
				}
				if (ImGui::MenuItem("Reset", 0, false, display->getChip8()->getActive())) {
					display->getChip8()->reset();
					display->getChip8()->loadProgram(display->getChip8()->getRomInfo().path); //check if path is valid?
					display->getChip8()->setActive(true);
					show = false;
				}
				if (ImGui::MenuItem("Shutdown", 0, false, display->getChip8()->getActive())) {
					display->getChip8()->setActive(false);
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Pause", 0, display->getChip8()->getPaused())) {
					display->getChip8()->setPaused(!display->getChip8()->getPaused());
				}
				if (ImGui::MenuItem("Frame Advance")) {
					display->getChip8()->setPaused(false);
					display->getChip8()->setFrameAdvance(true);
				}
				ImGui::Separator();
				if (ImGui::BeginMenu("System"))
				{
					if (ImGui::BeginMenu("Quirks")) {
						Chip8Quirks quirk = display->getChip8()->getQuirks();

						ImGui::Checkbox("Logic Quirk", &quirk.logicQuirks);
						ImGui::Checkbox("Clip Quirk", &quirk.clipQuirks);
						ImGui::Checkbox("Jump Quirk", &quirk.jumpQuirks);
						ImGui::Checkbox("Load/Store Quirk", &quirk.loadStoreQuirks);
						ImGui::Checkbox("Shift Quirk", &quirk.shiftQuirks);
						ImGui::Checkbox("vBlank Quirk", &quirk.vBlankQuirks);

						display->getChip8()->setQuirks(quirk);
						ImGui::EndMenu();
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Settings")) {
				if (ImGui::BeginMenu("Frame Size"))
				{
					for (int i = 1; i <= 20; i++) {
						if (ImGui::MenuItem(std::string(std::to_string(i) + "x").c_str())) {
							CONFIG.frameSize = i;

							int windowXSize = CONFIG.frameSize * 64;
							int windowYSize = CONFIG.frameSize * 32;

							SDL_SetWindowSize(display->getWindow(), windowXSize, windowYSize);
							SDL_RenderSetLogicalSize(display->getRenderer(), windowXSize, windowYSize);
						}
					}

					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Fullscreen", 0, CONFIG.fullscreen)) {
					CONFIG.fullscreen = !CONFIG.fullscreen;
					SDL_SetWindowFullscreen(display->getWindow(), CONFIG.fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
				}

				if (ImGui::BeginMenu("Color Settings"))
				{
					float fillColors[3];
					fillColors[0] = CONFIG.fillColor.r / 255.0f;
					fillColors[1] = CONFIG.fillColor.g / 255.0f;
					fillColors[2] = CONFIG.fillColor.b / 255.0f;

					float bgColors[3];
					bgColors[0] = CONFIG.backgroundColor.r / 255.0f;
					bgColors[1] = CONFIG.backgroundColor.g / 255.0f;
					bgColors[2] = CONFIG.backgroundColor.b / 255.0f;

					if (ImGui::ColorPicker3("Fill Color", fillColors)) {
						CONFIG.fillColor.r = static_cast<char>(fillColors[0] * 255);
						CONFIG.fillColor.g = static_cast<char>(fillColors[1] * 255);
						CONFIG.fillColor.b = static_cast<char>(fillColors[2] * 255);
					}
					if (ImGui::ColorPicker3("Background Color", bgColors)) {
						CONFIG.backgroundColor.r = static_cast<char>(bgColors[0] * 255);
						CONFIG.backgroundColor.g = static_cast<char>(bgColors[1] * 255);
						CONFIG.backgroundColor.b = static_cast<char>(bgColors[2] * 255);
					}
					ImGui::EndMenu();
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Mute", 0, CONFIG.muteAudio)) {
					CONFIG.muteAudio = !CONFIG.muteAudio;
				}
				ImGui::Separator();
				if (ImGui::BeginMenu("Speed Target"))
				{
					if (ImGui::DragInt("CPU Speed", &CONFIG.baseSpeed, 1, 1, 2000)) {
						display->getChip8()->setRefreshRate(CONFIG.baseSpeed);
					}

					if (ImGui::DragInt("Draw Speed", &CONFIG.baseDrawSpeed, 1, 1, 2000)) {
						display->getChip8()->setDisplayRefreshRate(CONFIG.baseDrawSpeed);
					}

					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}

			/*if (ImGui::BeginMenu("Debug")) {
				ImGui::EndMenu();
			}*/
			ImGui::EndMainMenuBar();
		}


		/* {
			static float f = 0.0f;
			static int counter = 0;

			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::Begin("YaCh8E - Main Menu", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

			ImGui::End();
		}*/

		//ImGui::ShowDemoWindow();
	}
}

std::string openFileDialog()
{
	std::string path;

#ifdef WIN32
	OPENFILENAME ofn;
	char fileName[MAX_PATH] = "";

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = "All Files\0*.*\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetOpenFileName(&ofn))
	{
		path = fileName; // is this efficient?
	}
#elif
	std::cout << "For fucks sake... unimplemented!";
#endif
	return path;
}
