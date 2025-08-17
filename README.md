# Simci

Simci is a prototype city simulation game developed during my university years as an experiment with C++, OpenGL, and 3D modeling.

## Build Instructions

### 1. Install Dependencies (macOS)

```bash
brew install clang-format sdl2 sdl2_ttf
```

### 2. Compile the Program

From the project root directory:

```bash
make
```

This will generate the `simci` executable in the same directory.

## Running the Program

Execute the compiled binary:

```bash
./simci
```

## Controls
Simci uses keyboard shortcuts to switch between editing modes and actions:

| Key            | Action                   |
| -------------- | ------------------------ |
| **Esc**        | Exit the program         |
| **Space**      | Center the camera        |
| **Arrow keys** | Move camera position     |
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
