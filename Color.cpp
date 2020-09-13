#include "Color.h"

#include <iostream>

int Color::TranslateColor(EColor color)
{
	switch (color)
	{
	case EColor::BLACK:
		return 0x0;
	case EColor::DARK_BLUE:
		return 0x1;
	case EColor::DARK_GREEN:
		return 0x2;
	case EColor::DARK_AQUA:
		return 0x3;
	case EColor::DARK_RED:
		return 0x4;
	case EColor::DARK_PURPLE:
		return 0x5;
	case EColor::GOLD:
		return 0x6;
	case EColor::GRAY:
		return 0x7;
	case EColor::DARK_GRAY:
		return 0x8;
	case EColor::BLUE:
		return 0x9;
	case EColor::GREEN:
		return 0xA;
	case EColor::AQUA:
		return 0xB;
	case EColor::RED:
		return 0xC;
	case EColor::LIGHT_PURPLE:
		return 0xD;
	case EColor::YELLOW:
		return 0xE;
	case EColor::WHITE:
		return 0xF;
	default:
		return 0x0;
	}
}

WORD Color::ComposeColorValue(EColor foreground, EColor background)
{
	return Color::TranslateColor(background) * 16 + Color::TranslateColor(foreground);
}

void Color::SetMode(HANDLE hstdout, EColor foreground, EColor background)
{
	SetConsoleTextAttribute(hstdout, Color::ComposeColorValue(foreground, background));
}
