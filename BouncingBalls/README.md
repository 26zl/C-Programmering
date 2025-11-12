
# Bouncing Balls
Simple SDL2 demo that spawns several sphere-like objects (triangle models), applies gravity, and lets them bounce around the window.

## Prerequisites

- SDL2 development libraries and headers installed
- A C compiler (gcc/clang)

## Build

1. Open a terminal in the src folder.
2. Run:

```bash
make
```

This produces an executable named `app`.

## Run

```bash
./app
```

Controls:

- Press ESC or close the window to exit. The program waits briefly before quitting so any messages printed to stderr can be read.

## Clean

```bash
make clean
```

Notes:

- If you are on Windows and have trouble with native SDL2 tooling, building and running inside WSL2 or a Linux VM can simplify setup.

