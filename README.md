[![en](https://img.shields.io/badge/lang-en-red.svg)](https://github.com/petr590/NCV/blob/master/README.md)
[![ru](https://img.shields.io/badge/lang-ru-blue.svg)](https://github.com/petr590/NCV/blob/master/README-ru.md)

# New Console image Viewer 2.0
It's a program for rendering images in the console. Rendering is done using the ncurses library.

## Building
First you need to install the libraries:
```console
sudo apt install ncurses libav libswscale sdl2 sdl-mixer
```

Then download the repository and compile the code:
```console
git clone --depth=1 https://github.com/petr590/NCV.git
cd NCV/
mkdir release && cd release/
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

To build in debug mode, replace '-DCMAKE_BUILD_TYPE=Release' with '-DCMAKE_BUILD_TYPE=Debug'

## Usage
The program accepts flags and the path to a file or directory. By default, the current directory is specified.
If you specify path to directory, the program will find all images, gifs and videos in supported formats and show the first file.
```console
./ncv [-b] [-e] [-p] [file or directory]
```

### Flags
- -b, --big-chars: use large characters (for small console scale)
- -e, --extended: use dual resolution (may be slower, especially for video)
- -p, --parallel: play video in parallel with decoding (may cause lags)

### Management
Switch images using the arrows or keys <kbd>A</kbd>/<kbd>D</kbd>.  
The output is <kbd>Esc</kbd>, <kbd>Q</kbd> or <kbd>Ctrl+C</kbd>.  
If the image is drawn in a corner or with errors, redraw it with <kbd>R</kbd>  

## Libraries used
- **ncursesw** - for displaying colored text on the screen
- **libav** - library for reading images, videos and audio
- **libswscale** - for video resizing
- **SDL2** and **SDL_mixer** - for audio output (feature in development)
