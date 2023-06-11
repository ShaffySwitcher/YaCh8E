#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <random>

namespace Random {
	static std::mt19937 randomEngine;

	inline void setRandomSeed(unsigned int seed) { randomEngine.seed(seed); }
	inline int getRandomInt() { std::uniform_int_distribution<int> distribution(INT_MIN, INT_MAX); return distribution(randomEngine); }
	inline float getRandomFloat() { std::uniform_real_distribution<float> distribution(0.0f, 1.0f); return distribution(randomEngine); }
	inline unsigned short getRandomIntRange(int min, int max) { std::uniform_int_distribution<int> distribution(min, max); return distribution(randomEngine); }
	inline float getRandomFloatRange(float min, float max) { std::uniform_real_distribution<float> distribution(min, max); return distribution(randomEngine); }
}
