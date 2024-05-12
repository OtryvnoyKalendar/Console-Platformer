#!/bin/bash
# версия скрипта: 1.2 sudo
clear

file_exists=false
# Проверяем наличие файла в текущей директории
if [ -f "main.out" ]; then
    rm main.out
    file_exists=true
fi

gcc main.c -o main.out -lm -lncurses

# Повторно проверяем наличие файла
if [ -f "main.out" ]; then	
	echo "ВОТ И НАЧИНАЕТСЯ ПРОГРАММА:"
	echo "---"
	sudo ./main.out
	echo "---"
	
    rm main.out
    
    if [ "$file_exists" = true ]; then
        echo "исполняемый файл был удалён дважды"
    else
        echo "исполняемый файл был удалён после завершения работы программы"
    fi
else
    if [ "$file_exists" = true ]; then
        echo "исполняемый файл был удалён только перед началом работы программы"
    else
        echo "исполняемых файлов найдено не было"
    fi
fi
