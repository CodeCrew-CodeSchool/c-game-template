# STEPS

1. Prerequisites:

a. Windows

b. Download [VS Code](https://code.visualstudio.com/download)

c. [Download Raylib](https://www.raylib.com/) and install it. It should end up in C:/raylib

2. Open main.c and hit F5 key

2. OR open the Run and Debug window and hit the Play Button. Make sure the text says Debug.

# BFT - Bitcoin Fixed This

Play through a platforming adventure where you collect Satoshis. Along your adventure to stack sats, you'll uncover the nefarious deeds of Uncle Sam. His eagles, agents, and bankers are trying to stop you!
Good luck, anon.


---------------------------------------------

# Here are Some Additional Build Steps if You're Curious

## Extent
Demonstrates a the build process of a **simple & portable** raylib application for Desktop / Web.

- (Static) assets passed via `--preload-file` for Web
- Dynamic files using IDBFS (browser storage)

## Usage
```sh
git clone https://github.com/manuel5975p/raylib-cmake-template.git
cd raylib-cmake-template
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug #Can also be =Release, =RelWithDebInfo, nothing defaults to Debug
make -j4
./main
```
### Usage for Web
Requires [emscripten](https://emscripten.org/docs/getting_started/downloads.html).

```sh
mkdir build_web && cd build_web
emcmake cmake .. -DCMAKE_BUILD_TYPE=Release -DPLATFORM=Web
make -j4
```
### Usage with VSCode

- Open the `raylib-cmake-template` directory in VSCode
- Make sure the "C/C++" extension as well "CMake Tools" are installed
- Also have a compiler installed (e.g. [the latest w64devkit](https://github.com/skeeto/w64devkit/releases))
- Hit the debug, build button or hit Ctrl+Shift+P for the command menu and run "CMake: Debug" or something similar

