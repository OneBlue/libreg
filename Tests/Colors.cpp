#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "Colors.h"

using libreg::test::Colors;

namespace
{
#ifdef _WIN32
  HANDLE hconsole = nullptr;
  WORD default_attributes = 0;

  inline void Init()
  {
    if (hconsole != nullptr)
      return;

    hconsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO infos;
    GetConsoleScreenBufferInfo(hconsole, &infos);

    default_attributes = infos.wAttributes;
  }
#else
  constexpr const char* tcode_green = "\x1b[32m";
  constexpr const char* tcode_red = "\x1b[31m";
  constexpr const char* tcode_blue = "\x1b[34m";
  constexpr const char* tcode_reset = "\x1b[0m";
  constexpr const char* tcode_bold = "\033[1m";
#endif
}

#ifdef _WIN32
const char* Colors::Green()
{
  Init();
  SetConsoleTextAttribute(hconsole, FOREGROUND_GREEN);
  return "";
}

const char* Colors::Blue()
{
  Init();
  SetConsoleTextAttribute(hconsole, FOREGROUND_BLUE);
  return "";
}

const char* Colors::Red()
{
  Init();
  SetConsoleTextAttribute(hconsole, FOREGROUND_RED);
  return "";
}

const char* Colors::Reset()
{
  Init();
  SetConsoleTextAttribute(hconsole, default_attributes);
  return "";
}

const char* Colors::Bold()
{
  Init();
  return "";
}

#else

const char* Colors::Green()
{
  return tcode_green;
}

const char* Colors::Blue()
{
  return tcode_blue;
}

const char* Colors::Red()
{
  return tcode_red;
}

const char* Colors::Reset()
{
  return tcode_reset;
}

const char* Colors::Bold()
{
  return tcode_bold;
}
#endif
