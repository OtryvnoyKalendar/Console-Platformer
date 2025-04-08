### Скриншоты игры
Скрины | Скрины
:--------------:|:--------------:
![screenshot: Main menu](https://github.com/OtryvnoyKalendar/Console-Platformer/blob/main/screenshots/screenshot%201.png) | ![screenshot: gameplay 1](https://github.com/OtryvnoyKalendar/Console-Platformer/blob/main/screenshots/screenshot%202.png)
![screenshot: gameplay 2](https://github.com/OtryvnoyKalendar/Console-Platformer/blob/main/screenshots/screenshot%204.png) | ![screenshot: gameplay 3](https://github.com/OtryvnoyKalendar/Console-Platformer/blob/main/screenshots/screenshot%205.png)

### Общее описание
Это консольная игра, которую я разрабатывал для практики языка C. Игра Batet (название придумала нейроссеть) является типичным представителем жанра платформер.

Игра сделана на основе видео с ютуб канала Begin End и адаптирована под Linux. Вот *ссылки на видео*:  
[Язык Си с нуля - Урок 29 - Пишем простую игру платформер](https://www.youtube.com/watch?v=7sTPrktLLBo&list=PLBOPkQsFLCR2DWRY74L03FmbRtz_Yy73_&index=30),  
[Язык Си с нуля - Урок 33 - Пишем игру платформер, добавляем врагов :-)](https://www.youtube.com/watch?v=8QDgZ3GNlt8&list=PLBOPkQsFLCR2DWRY74L03FmbRtz_Yy73_&index=34).

### Зависимости
- gcc
- ncurses

### Как играть
Чтобы начать играть, нужно открыть консоль и перейти в папку с проектом. Дальше просто выполните скрипт:
```sh
sh project_start
```
Так как игра для получения состояния клавиатуры использует файл `/dev/input/by-path/platform-i8042-serio-0-event-kbd`, то нужно будет ввести пароль для запуска игры от имени суперпользователя. Иначе играть не получиться.

- Управление:  
'Space' - прыжок  
'A' - влево  
'D' - вправо  
'Q' - поставить на паузу  
'E' - продолжить игру  
'Esc' - выйти из игры  

Если вы хотите **изменить код игры**, например изменив макрос `CHITS` на `1`, то вам не нужно ничего перекомпелировать и удалять вручную. После внесения изменений в файлы игры, просто снова запустите скрипт `project_start`, он всё сделает за вас.

### Технические подробности
В процессе написания игры я сделал свои хидеры, которые можно использовать для других проектов. Заголовочный файл `core.h` нужен для включения в консоли режима ncurses, а `keys.h` нужен для проверки нажатия нескольких клавиш одновременно.

Я добился следующих важных моментов:
1. в игре нет миганий, только символы немного съезжают
2. реализован `GetKeyState()` для Linux
