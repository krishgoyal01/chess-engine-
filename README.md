# Beginner Chess (C++)

A beginner-friendly chess project written in C++ with two ways to play:
- Console mode (text input)
- Windows GUI mode (Win32 + GDI+)

This project includes core move logic, turn handling, capture rules, castling, en passant, promotion, and undo support.

## Project Structure

- [beginer.cpp](beginer.cpp): Console entry point
- [gui.cpp](gui.cpp): Windows GUI entry point
- [commit.cpp](commit.cpp): Move validation and commit logic
- [pieces_move.cpp](pieces_move.cpp): Move generation and attack checks
- [undo.cpp](undo.cpp): Move history and undo
- [globals.cpp](globals.cpp): Global game state
- [c_h.hpp](c_h.hpp): Shared declarations
- [create_sprite_sheet.py](create_sprite_sheet.py): Generates chess sprite sheet image
- [chess_pieces.png](chess_pieces.png): Piece sprite sheet used by GUI
- [Makefile](Makefile): Build automation for console and GUI targets

## Features

- Standard 8x8 board representation
- Legal move generation by piece type
- Check detection (king attack checks)
- Castling support
- En passant support
- Pawn promotion support (GUI prompt)
- Undo support (move history stack .).
- Click-to-move GUI with highlighted candidate squares

## Run Existing Executables

If you already have the binaries in this repo:

- Console game: c.exe
- GUI game: gui.exe

You can run them directly from the project folder on Windows.

## Build From Source (Windows)

### Option 1: Makefile 

Build both executables: mingw32-make

Build only console version: mingw32-make c.exe

Build only GUI version: mingw32-make gui.exe

Run targets:
mingw32-make run-cli
mingw32-make run-gui

Clean build outputs: mingw32-make clean

### Option 2: MinGW g++

Build console version:

g++ -std=c++17 beginer.cpp commit.cpp pieces_move.cpp undo.cpp globals.cpp -o c.exe

Build GUI version:

g++ -std=c++17 gui.cpp commit.cpp pieces_move.cpp undo.cpp globals.cpp -lgdiplus -lgdi32 -o gui.exe

Notes:
- Keep chess_pieces.png in the same folder as gui.exe.
- If you get linker issues, ensure your MinGW toolchain supports Win32 GUI libraries.


## How To Play

### Console Mode

1. Start c.exe
2. Enter source square in file-rank format, example: e2
3. Enter target square, example: e4
4. Enter 00 as source input to quit
5. After each move, you can undo when prompted

### GUI Mode

- Left click a piece to select it
- Left click a target square to move
- Right click or Esc to cancel selection
- Ctrl+Z, U key, or Game > Undo to undo
- Promotion opens a popup choice menu

## Requirements

- Windows OS (GUI uses Win32 APIs)
- C++17 compiler
- For sprite regeneration: Python + Pillow (optional)

Install Pillow (optional, only for sprite generation):

pip install pillow

Then run:

python create_sprite_sheet.py

## Current Limitations

- No checkmate or stalemate game-end detection yet
- No AI opponent yet
- Global-state architecture 

## Learning Goals Covered

- Board-state management
- Rule-based move validation
- Undo via stack history
- Win32 event-driven GUI basics
- Integrating game logic with rendering/UI
