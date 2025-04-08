#ifndef KEYS_H
#define KEYS_H

#include <linux/input.h>

// перед закрытием игры
void BeforeCloseGame();

// одновременное нажатие клавиш
void RefreshKeyboardStatus();
int GetKeyState(int Key);

// Методы для начала работы с библиотекой и завершением работы

void OpenKeysMode();
void CloseKeysMode();

#endif // KEYS_H
