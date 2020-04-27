#include "pch.h"
#include "Utils.h"

float Utils::Lerp(float a, float b, float t) {
	return a * (1 - t) + b * t;
}

uint32_t Utils::Pack(uint8_t a, uint8_t b, uint8_t c) {
	return (a << 16) | (b << 8) | c;
}