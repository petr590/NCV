[![en](https://img.shields.io/badge/lang-en-red.svg)](https://github.com/petr590/NCV/blob/master/README.md)
[![ru](https://img.shields.io/badge/lang-ru-blue.svg)](https://github.com/petr590/NCV/blob/master/README-ru.md)

# New Console image Viewer
This is a program for rendering images in the console. Works with ncurses.
You can pass an argument to the program - an image or a folder with images.

## Assembly
Need to install ncurses (or pdcurses for windows)

```console
foo@bar:~$ git clone https://github.com/petr590/NCV.git
foo@bar:~$ cd NCV/
foo@bar:~/NCV$ mkdir release
foo@bar:~/NCV$ cd release/
foo@bar:~/NCV/release$ cmake -DCMAKE_BUILD_TYPE=Release ..
foo@bar:~/NCV/release$ make
```

To build in debug mode, replace 'release' with 'debug'.
The 'CMAKE_BUILD_TYPE=Windows' option is an assembly along with the PDCurses source code for MinGW.
The Windows build has not been tested, edit the CMakeLists.txt file for it.

## Libraries used
- **ncursesw** (there is a polyfill for ncurses to output text to wchar_t)
- **stb_image** (no need to install, comes with the program)

## Management
Switch images using the arrows or keys <kbd>A</kbd>/<kbd>D</kbd>.
The output is <kbd>Esc</kbd>, <kbd>Q</kbd> or <kbd>Ctrl+C</kbd>.
If the image is compressed in width, turn on the "half characters" mode by pressing <kbd>E</kbd> (each pixel is 2.5 characters wide, not 2 characters wide).
If the image is drawn in a corner or with errors, redraw it on <kbd>R</kbd>
<kbd>W</kbd> - brief information.
<kbd>S</kbd> - add/remove noise.
<kbd>F1</kbd> - management help.
