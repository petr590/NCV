[![en](https://img.shields.io/badge/lang-en-red.svg)](https://github.com/petr590/NCV/blob/master/README.md)
[![ru](https://img.shields.io/badge/lang-ru-blue.svg)](https://github.com/petr590/NCV/blob/master/README-ru.md)

# New Console image Viewer 2.0
This is a program for rendering images in the console. Works with ncurses.
You can pass an argument to the program - an image or a folder with images.

## Assembly
You need to install ncurses, libav, libswscale, sdl2, sdl-mixer

```console
git clone https://github.com/petr590/NCV.git
cd NCV/
mkdir release && cd release
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

To build in debug mode, replace '-DCMAKE_BUILD_TYPE=Release' with '-DCMAKE_BUILD_TYPE=Debug'

## Launch
```console
./ncv [-b] [-e] [-p] <file or directory>
```

## Management
Switch images using the arrows or keys <kbd>A</kbd>/<kbd>D</kbd>.  
Exit: <kbd>Esc</kbd>, <kbd>Q</kbd> or <kbd>Ctrl+C</kbd>.  
If the image is drawn in a corner or with errors, redraw it on <kbd>R</kbd>  

## Libraries used
- **ncursesw** - to display colored text on the screen
- **libav** - library for reading images, videos and audio
- **libswscale** - for video resizing
- **SDL2** and **SDL_mixer** - for audio output (feature in development)
