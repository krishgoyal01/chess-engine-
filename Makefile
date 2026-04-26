# GNU Makefile for Beginner Chess (Windows + MinGW)
# Usage:
#   mingw32-make            -> build both executables
#   mingw32-make c.exe      -> build console version
#   mingw32-make gui.exe    -> build GUI version
#   mingw32-make clean      -> remove build outputs

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
LDFLAGS :=

# Shared engine files (no main function here)
ENGINE_SRCS := commit.cpp pieces_move.cpp undo.cpp globals.cpp
ENGINE_OBJS := $(ENGINE_SRCS:.cpp=.o)

CONSOLE_SRC := beginer.cpp
GUI_SRC := gui.cpp

CONSOLE_EXE := c.exe
GUI_EXE := gui.exe

.PHONY: all clean rebuild run-cli run-gui help

all: $(CONSOLE_EXE) $(GUI_EXE)

$(CONSOLE_EXE): $(CONSOLE_SRC:.cpp=.o) $(ENGINE_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(GUI_EXE): $(GUI_SRC:.cpp=.o) $(ENGINE_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) -lgdiplus -lgdi32

%.o: %.cpp c_h.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run-cli: $(CONSOLE_EXE)
	./$(CONSOLE_EXE)

run-gui: $(GUI_EXE)
	./$(GUI_EXE)

rebuild: clean all

clean:
	-del /Q *.o 2>nul
	-del /Q $(CONSOLE_EXE) 2>nul
	-del /Q $(GUI_EXE) 2>nul

help:
	@echo Targets:
	@echo   all      - Build console and GUI executables
	@echo   c.exe    - Build console executable
	@echo   gui.exe  - Build GUI executable
	@echo   run-cli  - Run console executable
	@echo   run-gui  - Run GUI executable
	@echo   clean    - Remove object files and executables
	@echo   rebuild  - Clean and build all
