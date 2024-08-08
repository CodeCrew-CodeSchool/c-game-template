COMPILER = clang
SOURCE_LIBS = -Llib/
INCLUDE_PATHS = -Iinclude
OSX_OPT = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
CFILES = lib/libraylib.a src/*.c
OSX_OUT = -o "bin/game_osx"

build_osx:
	$(COMPILER) $(OSX_OPT) $(SOURCE_LIBS) $(INCLUDE_PATHS) $(CFILES) $(OSX_OUT) 