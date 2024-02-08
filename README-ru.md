[![en](https://img.shields.io/badge/lang-en-red.svg)](https://github.com/petr590/NCV/blob/master/README.md)
[![ru](https://img.shields.io/badge/lang-ru-blue.svg)](https://github.com/petr590/NCV/blob/master/README-ru.md)

# New Console image Viewer
Это программа для отрисовки изображений в консоли. Работает с ncurses.
Программе можно передать аргумент - изображение или папку с изображениями.

## Сборка
Необходимо установить ncurses (или pdcurses для windows)

```console
foo@bar:~$ git clone https://github.com/petr590/NCV.git
foo@bar:~$ cd NCV/
foo@bar:~/NCV$ mkdir release
foo@bar:~/NCV$ cd release/
foo@bar:~/NCV/release$ cmake -DCMAKE_BUILD_TYPE=Release ..
foo@bar:~/NCV/release$ make
```

Для сборки в режиме debug замените 'release' на 'debug'
Опция 'CMAKE_BUILD_TYPE=Windows' - это сборка вместе с исходниками PDCurses под MinGW.
Сборка под Windows не была протестирована, для неё отредактируйте файл CMakeLists.txt.

## Используемые библиотеки
- **ncursesw** (есть полифилл для ncurses для выведения текста в wchar_t)
- **stb_image** (не нужно устанавливать, поставляется вместе с программой)

## Управление
Переключение изображений стрелками либо клавишами <kbd>A</kbd>/<kbd>D</kbd>.  
Выход - <kbd>Esc</kbd>, <kbd>Q</kbd> или <kbd>Ctrl+C</kbd>.  
Если изображение сжато по ширине, включите режим "половинных символов" клавишей <kbd>E</kbd> (каждый пиксель не 2 символа в ширину, а 2.5).  
Если изображение отрисовывается в углу или с ошибками, перерисуйте его на <kbd>R</kbd>  
<kbd>W</kbd> - краткая информация.  
<kbd>S</kbd> - добавить/убрать шум.  
<kbd>F1</kbd> - справка по управлению.
