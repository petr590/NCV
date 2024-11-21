[![en](https://img.shields.io/badge/lang-en-red.svg)](https://github.com/petr590/NCV/blob/master/README.md)
[![ru](https://img.shields.io/badge/lang-ru-blue.svg)](https://github.com/petr590/NCV/blob/master/README-ru.md)

# New Console image Viewer 2.0
Это программа для отрисовки изображений в консоли. Работает с ncurses.
Программе можно передать аргумент - изображение или папку с изображениями.

## Сборка
Необходимо установить ncurses, libav, libswscale, sdl2, sdl-mixer

```console
git clone https://github.com/petr590/NCV.git
cd NCV/
mkdir release && cd release
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

Для сборки в режиме debug замените '-DCMAKE_BUILD_TYPE=Release' на '-DCMAKE_BUILD_TYPE=Debug'

## Запуск
```console
./ncv [-b] [-e] [-p] <file or directory>
```

## Управление
Переключение изображений стрелками или клавишами <kbd>A</kbd>/<kbd>D</kbd>.  
Выход - <kbd>Esc</kbd>, <kbd>Q</kbd> или <kbd>Ctrl+C</kbd>.  
Если изображение отрисовывается в углу или с ошибками, перерисуйте его на <kbd>R</kbd>  

## Используемые библиотеки
- **ncursesw** - для вывода цветного текста на экран
- **libav** - библиотека для чтения изображений, видео и аудио
- **libswscale** - для ресайзинга видео
- **SDL2** и **SDL_mixer** - для выода звука (функция в разработке)
