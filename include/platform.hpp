#ifndef PLATFORM_HPP
#define PLATFORM_HPP

/**
 * @author: { @Override } : 20260831
 *
 * Console setup. Only Windows needs anything: its console starts in a
 * legacy codepage that mangles the card glyphs, and ANSI escapes are off
 * unless you turn them on. Mac and Linux terminals already do both.
 **/

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

inline void initConsole() noexcept {
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);

  const HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD mode = 0;
  if (out != INVALID_HANDLE_VALUE && GetConsoleMode(out, &mode)) {
    SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
  }
#endif
}

#endif
