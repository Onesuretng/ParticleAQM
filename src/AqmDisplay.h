#pragma once

#include "Particle.h"

namespace AqmDisplay
{
void begin();
void update(int temp, int humidity, int pressure, const String &airQuality, float concentration);
}
