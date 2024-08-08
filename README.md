## Raylib Visual Studio Code C Template for macOS

A simple Raylib project template for macOS to run and debug using VS Code and the C Programming Language.

### Steps

1. run `make` from terminal to compile using the included [Makefile](/Makefile)
2. run the game with the command `./bin/game_osx` (the compiled game path)

### Notes

* If you are asked to select a compiler when debugging for the first time, select C/C++ Clang:
* Compilation info can be found in [.vscode/tasks.json](.vscode/tasks.json)
* The binary is compiled to the `bin` directory

### Alternative Steps with Apple Developer Tools

1. Install Apple Developer Tools / Xcode
2. Install the offical Microsoft [C/C++ Extension Pack for VS Code](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack)
3. Open this project in VS Code, navigate and open [src/main.c](/src/main.c)
4. Press `f5` to start debugging or press the play icon in the top right of the source window.
5. After compiling, the game executable will launch with the text "Raylib Says Hello!"



---
## Disclaimer

I created this project because I couldn't find much information on running Raylib outside of XCode on macOS. It comes with no guarantees or support and should be seen soley as a starting point to getting Raylib working in VS Code. 

If you have any suggesstions for improving it, let me know.

## Credit

Thank you [moopa](https://github.com/moopa/raylib-vscode-macos-template)