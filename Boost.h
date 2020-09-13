#pragma once

#include "Meal.h"

struct UBoost
{
    UBoost(EMeal type, int x, int y) : type(type), x(x), y(y) {};

    EMeal type;
    int x;
    int y;
};
