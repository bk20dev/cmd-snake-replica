#pragma once

#include "Color.h"

struct UCharacter {
    inline UCharacter(char h, EColor color) : h(h), color(color) { background = EColor::BLACK; };
    inline UCharacter(char h, EColor foreground, EColor background) : h(h), color(foreground), background(background) {};

    char h;
    EColor color;
    EColor background;
};
