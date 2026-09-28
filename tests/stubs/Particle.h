#pragma once

#include <cstdint>
#include <map>
#include <string>

using String = std::string;

constexpr int D2 = 2;
constexpr int D3 = 3;
constexpr int D4 = 4;
constexpr int D5 = 5;
constexpr int A0 = 19;
constexpr int A2 = 17;
constexpr int INPUT = 0;
constexpr int OUTPUT = 1;
constexpr int HIGH = 1;

namespace TestPins
{
extern std::map<int, int> modes;
extern std::map<int, int> levels;
}

inline void pinMode(int pin, int mode)
{
  TestPins::modes[pin] = mode;
}

inline void digitalWrite(int pin, int level)
{
  TestPins::levels[pin] = level;
}
