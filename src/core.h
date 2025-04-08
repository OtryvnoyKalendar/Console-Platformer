#ifndef CORE_H
#define CORE_H

#include <ncurses.h> // работа с выводом в консоль

// карта игры

#define MAP_HEIGHT	24
#define MAP_WIDTH	80

void ErrorCloseGame(const char* message);
void CloseGame();

// сигналы

void CheckSignals();
void OpenNcursesMode();
void CloseNcursesMode();

// цвета

#define COLOR_BLACK		0
#define COLOR_RED		1
#define COLOR_GREEN		2
#define COLOR_YELLOW	3
#define COLOR_BLUE		4
#define COLOR_MAGENTA	5
#define COLOR_CYAN		6
#define COLOR_WHITE		7

void setBackgroundColor(int BackgroundColor);

// изминения консоли

void setConsoleSize(int consoleWidth, int consoleHeight);

#endif // CORE_H
