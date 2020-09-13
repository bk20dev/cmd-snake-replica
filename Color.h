#pragma once

#include <string>
#include <Windows.h>

enum class EColor : uint8_t {
	BLACK,
	DARK_BLUE,
	DARK_GREEN,
	DARK_AQUA,
	DARK_RED,
	DARK_PURPLE,
	GOLD,
	GRAY,
	DARK_GRAY,
	BLUE,
	GREEN,
	AQUA,
	RED,
	LIGHT_PURPLE,
	YELLOW,
	WHITE
};

namespace Color {
	int TranslateColor(EColor color);
	WORD ComposeColorValue(EColor foreground = EColor::WHITE, EColor background = EColor::BLACK);

	void SetMode(HANDLE hstdout, EColor foreground = EColor::WHITE, EColor background = EColor::BLACK);
};
