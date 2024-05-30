#ifndef KEYS_H
#define KEYS_H

// перед закрытием игры
void BeforeCloseGame();


// одновременное нажатие клавиш

#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <linux/kernel.h>

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
#define BITS_PER_BYTE 8
#define BITS_TO_LONGS(nr) DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))

static inline int is_bit_set(unsigned long *array, int bit) {
    return (array[bit / (sizeof(long) * 8)] & (1UL << (bit % (sizeof(long) * 8)))) != 0;
}

int fd, ret;
unsigned long key_state[BITS_TO_LONGS(KEY_CNT)];

// Методы для начала работы с библиотекой и завершением работы
void OpenKeysMode() {
	fd = open("/dev/input/by-path/platform-i8042-serio-0-event-kbd", O_RDONLY);
	if (fd == -1)
		ErrorCloseGame("не получилось открыть устройство клавиатуры");
}

void CloseKeysMode() {
	close(fd);
}

// Получаем состояние клавиатуры
void RefreshKeyboardStatus() {
	ret = ioctl(fd, EVIOCGKEY(sizeof(key_state)), key_state);
	if (ret < 0)
		ErrorCloseGame("не получилось получить состояния клавиатуры");
}

int GetKeyState(int Key) {
	return (is_bit_set(key_state, Key));
}

#endif // KEYS_H
