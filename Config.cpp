#include "Config.h"
#include <iostream>

// Here is C++'s weakpoint...

void Config::loadConfig()
{
	mINI::INIFile file("config.ini");
	mINI::INIStructure ini;

	file.read(ini);

	keyboardLayout[1] = stoi(ini["keyboard"]["1_key"]);
	keyboardLayout[2] = stoi(ini["keyboard"]["2_key"]);
	keyboardLayout[3] = stoi(ini["keyboard"]["3_key"]);
	keyboardLayout[0xc] = stoi(ini["keyboard"]["c_key"]);
	keyboardLayout[4] = stoi(ini["keyboard"]["4_key"]);
	keyboardLayout[5] = stoi(ini["keyboard"]["5_key"]);
	keyboardLayout[6] = stoi(ini["keyboard"]["6_key"]);
	keyboardLayout[0xd] = stoi(ini["keyboard"]["d_key"]);
	keyboardLayout[7] = stoi(ini["keyboard"]["7_key"]);
	keyboardLayout[8] = stoi(ini["keyboard"]["8_key"]);
	keyboardLayout[9] = stoi(ini["keyboard"]["9_key"]);
	keyboardLayout[0xe] = stoi(ini["keyboard"]["e_key"]);
	keyboardLayout[0xa] = stoi(ini["keyboard"]["a_key"]);
	keyboardLayout[0] = stoi(ini["keyboard"]["0_key"]);
	keyboardLayout[0xb] = stoi(ini["keyboard"]["b_key"]);
	keyboardLayout[0xf] = stoi(ini["keyboard"]["f_key"]);

	system = stoi(ini["system"]["system"]);
	baseSpeed = stoi(ini["system"]["tickspeed"]);
	baseDrawSpeed = stoi(ini["system"]["drawspeed"]);

	std::string fillColorString = ini["display"]["fillcolor"];
	std::string bgColorString = ini["display"]["backgroundcolor"];

	fillColorString.erase(0, 1);
	bgColorString.erase(0, 1);

	fillColor.r = stoi(fillColorString.substr(0, 2), 0, 16);
	fillColor.g = stoi(fillColorString.substr(2, 2), 0, 16);
	fillColor.b = stoi(fillColorString.substr(4, 2), 0, 16);

	backgroundColor.r = stoi(bgColorString.substr(0, 2), 0, 16);
	backgroundColor.g = stoi(bgColorString.substr(2, 2), 0, 16);
	backgroundColor.b = stoi(bgColorString.substr(4, 2), 0, 16);

	frameSize = stoi(ini["display"]["framesize"]);
	fullscreen = stoi(ini["display"]["fullscreen"]);

	muteAudio = stoi(ini["audio"]["mute"]);

	file.write(ini);
}

void Config::saveConfig()
{
	mINI::INIFile file("config.ini");

	mINI::INIStructure ini;

	ini["keyboard"]["0_key"] = std::to_string(keyboardLayout[0]);
	ini["keyboard"]["1_key"] = std::to_string(keyboardLayout[1]);
	ini["keyboard"]["2_key"] = std::to_string(keyboardLayout[2]);
	ini["keyboard"]["3_key"] = std::to_string(keyboardLayout[3]);
	ini["keyboard"]["4_key"] = std::to_string(keyboardLayout[4]);
	ini["keyboard"]["5_key"] = std::to_string(keyboardLayout[5]);
	ini["keyboard"]["6_key"] = std::to_string(keyboardLayout[6]);
	ini["keyboard"]["7_key"] = std::to_string(keyboardLayout[7]);
	ini["keyboard"]["8_key"] = std::to_string(keyboardLayout[8]);
	ini["keyboard"]["9_key"] = std::to_string(keyboardLayout[9]);
	ini["keyboard"]["a_key"] = std::to_string(keyboardLayout[0xa]);
	ini["keyboard"]["b_key"] = std::to_string(keyboardLayout[0xb]);
	ini["keyboard"]["c_key"] = std::to_string(keyboardLayout[0xc]);
	ini["keyboard"]["d_key"] = std::to_string(keyboardLayout[0xd]);
	ini["keyboard"]["e_key"] = std::to_string(keyboardLayout[0xe]);
	ini["keyboard"]["f_key"] = std::to_string(keyboardLayout[0xf]);

	ini["system"]["system"] = std::to_string(system);
	ini["system"]["tickspeed"] = std::to_string(baseSpeed);
	ini["system"]["drawspeed"] = std::to_string(baseDrawSpeed);

	std::ostringstream stream;
	stream << "#" << std::setfill('0') << std::setw(2) << std::hex << (int)backgroundColor.r
		<< std::setw(2) << std::hex << (int)backgroundColor.g << std::setw(2) << std::hex << (int)backgroundColor.b;
	ini["display"]["backgroundcolor"] = stream.str();

	stream = std::ostringstream();
	stream << "#" << std::setfill('0') << std::setw(2) << std::hex << (int)fillColor.r
		<< std::setw(2) << std::hex << (int)fillColor.g << std::setw(2) << std::hex << (int)fillColor.b;
	ini["display"]["fillcolor"] = stream.str();

	ini["display"]["framesize"] = std::to_string(frameSize);
	ini["display"]["fullscreen"] = std::to_string(fullscreen);

	ini["audio"]["mute"] = std::to_string(muteAudio);

	file.write(ini);
}
