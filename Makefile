# Runnable file name
TARGET = simci

# Source code files (all .cpp, used for compilation)
SRC = *.cpp

# Files to format/check — excludes the third-party l3ds library
FMT_SRC = $(filter-out l3ds.cpp, $(wildcard *.cpp))
FMT_HDR = $(filter-out l3ds.h,   $(wildcard *.h))

# Compiler
CXX = g++

# Detect OS
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)  # macOS
    BREW_PREFIX := $(shell brew --prefix)
    PKGCONFIG_EXISTS := $(shell command -v pkg-config 2>/dev/null)

    ifeq ($(PKGCONFIG_EXISTS),)
        CXXFLAGS = -std=c++17 -Wall -I$(BREW_PREFIX)/include
        LDFLAGS  = -L$(BREW_PREFIX)/lib -lSDL2 -lSDL2_ttf -framework OpenGL -DGL_SILENCE_DEPRECATION
    else
        CXXFLAGS = -std=c++17 -Wall $(shell pkg-config --cflags sdl2)
        LDFLAGS  = $(shell pkg-config --libs sdl2) -lSDL2_ttf -framework OpenGL -DGL_SILENCE_DEPRECATION
    endif
else ifeq ($(UNAME_S),Linux)  # Ubuntu/Linux
    CXXFLAGS = -std=c++17 -Wall $(shell pkg-config --cflags sdl2 SDL2_ttf)
    LDFLAGS  = $(shell pkg-config --libs sdl2 SDL2_ttf) -lGL -lGLU
endif

# Default rule
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(SRC) -o $(TARGET) $(CXXFLAGS) $(LDFLAGS)

# Clean binaries
clean:
	rm -f $(TARGET)

# Format all C++ source files in place (l3ds excluded — third-party library)
format:
	clang-format -i $(FMT_SRC) $(FMT_HDR)

# Optional: check formatting without changing files
format-check:
	clang-format --dry-run --Werror $(FMT_SRC) $(FMT_HDR)

.PHONY: all clean format format-check
