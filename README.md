# Simci [![Build](https://github.com/gillarramendi/simci/actions/workflows/build.yml/badge.svg)](https://github.com/gillarramendi/simci/actions/workflows/build.yml)

Simci is a prototype city simulation game developed during my university years as an experiment with C++, OpenGL, and 3D modeling.

## Build Instructions

### 1. Install Dependencies

**macOS**
```bash
brew install clang-format sdl2 sdl2_ttf
```

**Linux**
```bash
sudo apt-get update
sudo apt-get install build-essential clang-format libsdl2-dev libsdl2-ttf-dev libglu1-mesa-dev
```

### 2. Compile

```bash
make
```

This produces the `simci` executable in the project root.

## Running

```bash
./simci
```

## Other Makefile Targets

| Target             | Description                          |
| ------------------ | ------------------------------------ |
| `make clean`       | Remove the compiled binary           |
| `make format`      | Auto-format all source files         |
| `make format-check`| Check formatting without modifying files |
| `make clean all format` | Full clean rebuild with formatting |

## Controls

| Key            | Action                   |
| -------------- | ------------------------ |
| **Esc**        | Exit the program         |
| **Space**      | Center the camera        |
| **Arrow keys** | Move camera              |
| **+**          | Move camera up           |
| **-**          | Move camera down         |
| **e**          | Default mode (no action) |
| **u**          | Raise terrain            |
| **d**          | Lower terrain            |
| **l**          | Level terrain            |
| **r**          | Residential zone         |
| **c**          | Commercial zone          |
| **i**          | Industrial zone          |
| **x**          | Undo last action         |
| **b**          | Build                    |
| **k**          | Place road               |
