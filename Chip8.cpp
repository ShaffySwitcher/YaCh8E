#include "Chip8.h"

const std::array<uint8_t, 80> defaultFont = {
  0xf0, 0x90, 0x90, 0x90, 0xf0, 0x20, 0x60, 0x20, 0x20, 0x70, 0xf0, 0x10, 0xf0, 0x80, 0xf0, 0xf0,
  0x10, 0xf0, 0x10, 0xf0, 0x90, 0x90, 0xf0, 0x10, 0x10, 0xf0, 0x80, 0xf0, 0x10, 0xf0, 0xf0, 0x80,
  0xf0, 0x90, 0xf0, 0xf0, 0x10, 0x20, 0x40, 0x40, 0xf0, 0x90, 0xf0, 0x90, 0xf0, 0xf0, 0x90, 0xf0,
  0x10, 0xf0, 0xf0, 0x90, 0xf0, 0x90, 0x90, 0xe0, 0x90, 0xe0, 0x90, 0xe0, 0xf0, 0x80, 0x80, 0x80,
  0xf0, 0xe0, 0x90, 0x90, 0x90, 0xe0, 0xf0, 0x80, 0xf0, 0x80, 0xf0, 0xf0, 0x80, 0xf0, 0x80, 0x80
};

Chip8::Chip8()
{
	reset();
	loadFont();
}

Chip8::~Chip8()
{
}

void Chip8::reset()
{
	setActive(false);
	setFrameAdvance(false);
	
	Random::setRandomSeed(static_cast<int>(time(NULL)));

	pc = 0x200;
	i = 0;
	std::fill(ram.begin()+80, ram.end(), 0); // don't clear font data
	std::fill(vram.begin(), vram.end(), 0);
	std::fill(keys.begin(), keys.end(), false);
	stack = std::stack<uint16_t>();
	delayTimer = 0;
	soundTimer = 0;

	switch (CONFIG.system) {
	case 0:
		logicQuirks = false;
		loadStoreQuirks = true;
		vBlankQuirks = true;
		clipQuirks = true;
		shiftQuirks = false;
		jumpQuirks = false;
	}

	refreshRate = CONFIG.baseSpeed;
	displayRefreshRate = CONFIG.baseDrawSpeed;
}

void Chip8::clearScreen()
{
	std::fill(vram.begin(), vram.end(), 0);
}

void Chip8::loadFont()
{
	std::ifstream fontData("font.bin", std::ios::in | std::ios::binary);
	if (fontData.good()) {
		fontData.read((char*)ram.data(), 80);
	}
	else {
		std::cout << "No font.bin found! Using fallback font data!" << std::endl;
		std::copy(defaultFont.begin(), defaultFont.end(), ram.data());
	}
}

void Chip8::loadProgram(std::string path)
{
	this->romInfo.path = path;
	std::ifstream romData(path, std::ifstream::ate | std::ifstream::binary);
	if (romData.good()) {
		std::streamoff romSize = romData.tellg();
		this->romInfo.size = romSize;
		romData.close();

		romData = std::ifstream(path, std::ios::in | std::ios::binary);
		romData.read((char*)ram.data() + pc, std::min<std::streamoff>(romSize, 4096));

		romData.close();
	}
	else {
		std::cout << "Couldn't open \"" << path << "\"!" << std::endl;
		std::copy(defaultFont.begin(), defaultFont.end(), ram.data());
	}
}

void Chip8::updateTimers()
{
	if ((running) && (!paused)) { //shoudTick() private function?
		if (soundTimer > 0) {
			soundTimer--;
		}
		else {
			SDL_PauseAudio(1);
		}

		if (delayTimer > 0) {
			delayTimer--;
		}
	}
}

void Chip8::update()
{
	if ((running)&&(!paused)) {
		uint16_t opcode = Memory::readWord(ram, pc);

		if (vBlankQuirks && (((opcode & 0xF000) == 0xD000) && drawFlag)) { // V-blank quirk - wait drawing before another drawing!
			return;
		}

		pc = (pc + 2) & 0x0FFF;

		switch (opcode & 0xF000) {
		case 0x0000:
			switch (opcode & 0x00FF) {
			case 0x0000:
				//nop
				break;
			case 0x00E0:
				clearScreen();
				break;
			case 0x00EE:
				pc = stack.top();
				stack.pop();
				break;
			default:
				std::cout << std::hex << "Unknown opcode (" << opcode << ")\n";
				break;
			}
			break;
		case 0x1000:
			pc = opcode & 0x0FFF;
			break;
		case 0x2000:
			stack.push(pc);
			pc = opcode & 0x0FFF;
			break;
		case 0x3000:
			if (registers[(opcode & 0x0F00) >> 8] == static_cast<uint8_t>(opcode & 0x00FF)) {
				pc += 2;
			}
			break;
		case 0x4000:
			if (registers[(opcode & 0x0F00) >> 8] != static_cast<uint8_t>(opcode & 0x00FF)) {
				pc += 2;
			}
			break;
		case 0x5000:
			if (registers[(opcode & 0x0F00) >> 8] == registers[(opcode & 0x00F0) >> 4]) {
				pc += 2;
			}
			break;
		case 0x6000:
			registers[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
			break;
		case 0x7000:
			registers[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
			break;
		case 0x8000:
			switch (opcode & 0x000F) {
			case 0x0000:
				registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x00F0) >> 4];
				break;
			case 0x0001:
				if (logicQuirks) {
					registers[0xF] = 0;
				}
				registers[(opcode & 0x0F00) >> 8] |= registers[(opcode & 0x00F0) >> 4];
				break;
			case 0x0002:
				if (logicQuirks) {
					registers[0xF] = 0;
				}
				registers[(opcode & 0x0F00) >> 8] &= registers[(opcode & 0x00F0) >> 4];
				break;
			case 0x0003:
				if (logicQuirks) {
					registers[0xF] = 0;
				}
				registers[(opcode & 0x0F00) >> 8] ^= registers[(opcode & 0x00F0) >> 4];
				break;
			case 0x0004:
			{
				uint16_t sum = registers[(opcode & 0x0F00) >> 8] + registers[(opcode & 0x00F0) >> 4];

				registers[(opcode & 0x0F00) >> 8] = sum & 0xFF;
				registers[0xF] = (sum > 255) ? 1 : 0;
			}
			break;
			case 0x0005:
			{
				uint16_t difference = registers[(opcode & 0x0F00) >> 8] - registers[(opcode & 0x00F0) >> 4];

				registers[(opcode & 0x0F00) >> 8] = difference & 0xFF;
				registers[0xF] = (difference > 255) ? 0 : 1;
			}
			break;
			case 0x0006:
			{
				if (!shiftQuirks) {
					registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x00F0) >> 4];
				}

				uint16_t shift = registers[(opcode & 0x0F00) >> 8];

				registers[(opcode & 0x0F00) >> 8] = shift >> 1;
				registers[0xF] = shift & 0x1;
			}
			break;
			case 0x0007:
			{
				uint16_t difference = registers[(opcode & 0x00F0) >> 4] - registers[(opcode & 0x0F00) >> 8];

				registers[(opcode & 0x0F00) >> 8] = difference & 0xFF;
				registers[0xF] = (difference > 255) ? 0 : 1;
			}
			break;
			case 0x000E:
			{
				if (!shiftQuirks) {
					registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x00F0) >> 4];
				}

				uint16_t shift = registers[(opcode & 0x0F00) >> 8];

				registers[(opcode & 0x0F00) >> 8] = shift << 1;
				registers[0xF] = shift >> 7;
			}
			break;
			default:
				std::cout << std::hex << "Unknown opcode (" << opcode << ")\n";
				break;
			}
			break;
		case 0x9000:
			if (registers[(opcode & 0x0F00) >> 8] != registers[(opcode & 0x00F0) >> 4]) {
				pc += 2;
			}
			break;
		case 0xA000:
			i = opcode & 0x0FFF;
			break;
		case 0xB000:
			pc = (opcode & 0x0FFF) + registers[jumpQuirks ? ((opcode & 0x0F00) >> 8) : 0];
			break;
		case 0xC000:
			registers[(opcode & 0x0F00) >> 8] = Random::getRandomInt() & (opcode & 0x00FF);
			break;
		case 0xD000:
		{
			drawFlag = true;

			uint8_t x = registers[(opcode & 0x0F00) >> 8] % 64;
			uint8_t y = registers[(opcode & 0x00F0) >> 4] % 32;
			uint8_t height = opcode & 0x000F;

			registers[0xF] = 0;

			for (int h = 0; h < height; h++) {
				for (int w = 0; w < 8; w++) {
					uint16_t vramPosition = ((x + w) % 64) + ((y + h) % 32) * 64;
					bool currentPixel = ((ram[i + h] >> (7 - w)) & 0x1) != 0;
					if ((x + w >= 64 || y + h >= 32) && clipQuirks) { currentPixel = false; } // if sprite is outside boundary it wraps otherwise don't draw anything else
					if (!currentPixel) { continue; }
					if (vram[vramPosition])
					{
						vram[vramPosition] = 0;
						registers[0xF] = 1;
					}
					else {
						vram[vramPosition] = 1;
					}
				}
			}
		}
		break;
		case 0xE000:
			switch (opcode & 0x00FF) {
			case 0x009E:
				if (keys[registers[(opcode & 0x0F00) >> 8]]) {
					pc += 2;
				}
				break;
			case 0x00A1:
				if (!keys[registers[(opcode & 0x0F00) >> 8]]) {
					pc += 2;
				}
				break;
			default:
				std::cout << std::hex << "Unknown opcode (" << opcode << ")\n";
				break;
			}
			break;
		case 0xF000:
			switch (opcode & 0x00FF) {
			case 0x0007:
				registers[(opcode & 0x0F00) >> 8] = delayTimer;
				break;
			case 0x000A:
				pc -= 2; // this might cause a glitch in REALLY specific cases

				for (auto i = 0; i < 16; i++) {
					if (keys[i]) {
						pc += 2;
						break;
					}
				}
				break;
			case 0x0015:
				delayTimer = registers[(opcode & 0x0F00) >> 8];
				break;
			case 0x0018:
				soundTimer = registers[(opcode & 0x0F00) >> 8];
				if (soundTimer > 0) {
					if (!CONFIG.muteAudio) {
						SDL_PauseAudio(0);
					}
				}
				break;
			case 0x001E:
				i += registers[(opcode & 0x0F00) >> 8];
				break;
			case 0x0029:
				i = registers[(opcode & 0x0F00) >> 8] * 0x5;
				break;
			case 0x0033:
				ram[i] = registers[(opcode & 0x0F00) >> 8] / 100;
				ram[i + 1] = (registers[(opcode & 0x0F00) >> 8] / 10) % 10;
				ram[i + 2] = (registers[(opcode & 0x0F00) >> 8] % 100) % 10;
				break;
			case 0x0055:
				for (auto j = 0; j <= ((opcode & 0x0F00) >> 8); ++j) {
					ram[i + j] = registers[j];
				}

				if (loadStoreQuirks) {
					i += ((opcode & 0x0F00) >> 8) + 1;
				}

				break;
			case 0x0065:
				for (auto j = 0; j <= ((opcode & 0x0F00) >> 8); ++j) {
					registers[j] = ram[i + j];
				}

				if (loadStoreQuirks) {
					i += ((opcode & 0x0F00) >> 8) + 1;
				}

				break;
			default:
				std::cout << std::hex << "Unknown opcode (" << opcode << ")\n";
				break;
			}
			break;
		default:
			std::cout << std::hex << "Unknown opcode (" << opcode << ")\n";
			break;
		}
	}
}

void Chip8::setQuirks(Chip8Quirks quirks)
{
	logicQuirks = quirks.logicQuirks;
	loadStoreQuirks = quirks.loadStoreQuirks;
	vBlankQuirks = quirks.vBlankQuirks;
	clipQuirks = quirks.clipQuirks;
	shiftQuirks = quirks.shiftQuirks;
	jumpQuirks = quirks.jumpQuirks;
}

Chip8Quirks Chip8::getQuirks()
{
	Chip8Quirks quirks = {logicQuirks, loadStoreQuirks, vBlankQuirks, clipQuirks, shiftQuirks, jumpQuirks };
	return quirks;
}
