CC = gcc -g
FLAGS = -lncurses -lpthread -o 
DEBUG_FLAGS = -lncurses -lpthread -O0 -g -o

BIN = TCP.c main.c GenerateMap.c GameplayPacman.c
EXE = main.exe

# TODO after start of program
help:
	@echo "Usage:"

all: $(EXE)
	@./$(EXE)

build: $(EXE)
	@echo "Building project..."

debug: $(BIN)
	@$(CC) $(BIN) $(DEBUG_FLAGS) $(EXE)

leap: debug
	@valgrind ./$(EXE)

$(EXE): $(BIN)
	@echo "Compiling files..."
	@$(CC) $(BIN) $(FLAGS) $(EXE)

clean:
	@rm -vf $(EXE)

clear:
	@rm -vf $(EXE)
