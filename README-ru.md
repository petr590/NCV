[![en](https://img.shields.io/badge/lang-en-red.svg)](https://github.com/petr590/NCV/blob/master/README.md)
[![ru](https://img.shields.io/badge/lang-ru-blue.svg)](https://github.com/petr590/NCV/blob/master/README-ru.md)

# New Console image Viewer 2.0
Это программа для отрисовки изображений в консоли. Отрисовка происходит с помощью библиотеки ncurses.

## Сборка
Сначала необходимо установить библиотеки:
```console
sudo apt install ncurses libav libswscale sdl2 sdl-mixer
```

Затем скачать репозиторий и скомпилировать код:
```console
git clone --depth=1 https://github.com/petr590/NCV.git
cd NCV/
mkdir release && cd release/
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

Для сборки в режиме debug замените '-DCMAKE_BUILD_TYPE=Release' на '-DCMAKE_BUILD_TYPE=Debug'

## Использование
Программа принимает флаги и путь к файлу или папке. По умолчанию указывается текущая папка.
Если указать путь к папке, то программа найдёт все изображения, gif и видео в поддерживаемых форматах и покажет первый файл.
```console
./ncv [-b] [-e] [-p] [file or directory]
```

### Флаги
- -b, --big-chars:  использовать большие символы (для маленького масштаба консоли)
- -e, --extended:   использовать двойное разрешение (может работать медленнее, особенно для видео)
- -p, --parallel:   воспроизводить видео параллельно с декодированием (может вызывать лаги)

### Управление
Переключение изображений стрелками или клавишами <kbd>A</kbd>/<kbd>D</kbd>.  
Выход - <kbd>Esc</kbd>, <kbd>Q</kbd> или <kbd>Ctrl+C</kbd>.  
Если изображение отрисовывается в углу или с ошибками, перерисуйте его с помощью <kbd>R</kbd>  

## Используемые библиотеки
- **ncursesw** - для вывода цветного текста на экран
- **libav** - библиотека для чтения изображений, видео и аудио
- **libswscale** - для ресайзинга видео
- **SDL2** и **SDL_mixer** - для выода звука (функция в разработке)
