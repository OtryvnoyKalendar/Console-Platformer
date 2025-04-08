#include <signal.h> // signal
#include <stdio.h> // printf
#include <stdlib.h> // exit

#include "core.h"

// закрытие игры

void BeforeCloseGame();

void ErrorCloseGame(const char* message) {
	BeforeCloseGame();
	printf("Ошибка: %s\n", message);
	exit(1);
}

void CloseGame() {
	BeforeCloseGame();
	exit(1);
}

// сигналы

volatile sig_atomic_t signal_status = 0;

void sighandler(int s) {
	signal_status = s;
}

void setSignals() {
	signal(SIGINT, sighandler); // когда нажимаем Ctrl+C
	signal(SIGQUIT, sighandler); // когда нажимаем Ctrl+'\'
	signal(SIGWINCH, sighandler); // когда изменяется размер терминала
	signal(SIGTSTP, sighandler); // когда нажимаем Ctrl+Z
}

void CheckSignals() {
	if (signal_status && signal_status != SIGWINCH)
		CloseGame();
}

// цвета

void setBackgroundColor(int BackgroundColor) {
	init_pair(COLOR_BLACK, COLOR_BLACK, BackgroundColor);
	init_pair(COLOR_GREEN, COLOR_GREEN, BackgroundColor);
	init_pair(COLOR_WHITE, COLOR_WHITE, BackgroundColor);
	init_pair(COLOR_RED, COLOR_RED, 	BackgroundColor);
	init_pair(COLOR_CYAN, COLOR_CYAN, 	BackgroundColor);
	init_pair(COLOR_MAGENTA, COLOR_MAGENTA, BackgroundColor);
	init_pair(COLOR_BLUE, COLOR_BLUE, 	BackgroundColor);
	init_pair(COLOR_YELLOW, COLOR_YELLOW, BackgroundColor);
}

void includeColors() {
	if (has_colors()) {
		start_color();
		setBackgroundColor(COLOR_BLACK);
	}
	else
		ErrorCloseGame("терминал не поддерживает цвета");
}

// смена режима ncurses

void OpenNcursesMode() {
	initscr();
	savetty();
	nonl();
	cbreak();
	noecho();
	timeout(0);
	leaveok(stdscr, TRUE);
	curs_set(0);
	
	includeColors();
	setSignals();
}

void CloseNcursesMode() {
	curs_set(1);
	clear();
	refresh();
	resetty();
	endwin();
}

// изминения консоли

void setConsoleSize(int consoleWidth, int consoleHeight) {
	printf("\e[8;%d;%dt", consoleHeight, consoleWidth);
}

