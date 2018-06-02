# opengl-on-wayland

I wanted to use Wayland for creating an OpenGL context. But it's not as straightforward as expected. I'm using [SDL2](http://libsdl.org) for window creation and [GLEW](http://glew.sourceforge.net) as OpenGL loading library.

## Step 1 – Compile SDL2

First install the build dependencies as written in `docs/README-linux.md`. Then you can just call `configure`:

```
mkdir build
cd build
../configure --without-x --prefix=$HOME/.local
make
make install
```

`--without-x` and `--prefix` are not really necessary:
* The former is just to make sure we're really using Wayland instead of X11.
* I used the latter because I wanted to install the libraries locally without root rights.

## Step 2 – Compile GLEW

The necessary dependencies are at the end of `doc/build.html`. Next you just need to run make with the right arguments:

```
make SYSTEM=linux-egl GLEW_DEST=$HOME/.local
make install
```

* `SYSTEM=linux-egl` is really necessary if you don't want an unknown error when initializing GLEW.
* Setting `GLEW_DEST` is the same as setting `--prefix` for SDL2, you get a local installation.

## Step 3 – Compile

Now you just need to link to your new libraries and you're ready! I like to use CMake instead of raw Makefiles so you might want to check out my `CMakeLists.txt`.

```
git clone https://github.com/leyhline/opengl-on-wayland.git
cd opengl-on-wayland
mkdir build
cd build
cmake ..
make
./main
```

You might need to edit your `CMakeLists.txt` to point to your newly compiled libraries if you installed them locally (or use `cmake -D`):

```
GLEW_INCLUDE_DIR:PATH=/home/your-username/.local/include
GLEW_LIBRARY:FILEPATH=/home/your-username/.local/lib64/libGLEW.so
SDL_INCLUDE_DIR:PATH=/home/your-username/.local/include
SDL_LIBRARY:FILEPATH=/home/your-username/.local/lib/libSDL2.so
```

If everything worked out you'll see this nice triangle. You'll also might want to check out the `check_for_wayland` function in `main.c` which uses SDL to query information about the window manager.

![Hello Wayland](hello-world-triangle.png)
