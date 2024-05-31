CC = gcc
FLAGS = -lncurses -lpthread -o
DEBUG_FLAGS = -lncurses -lpthread -O0 -g -o

BIN = TCP.c main.c GenerateMap.c GameplayPacman.c
HEADER = header.h
EXE = main.exe

LOGS = logs*.txt

help:
	@echo "Usage: make build"
		  "server: make server ARGS=\"-p port -c count_of_players -n Name\""
		  "client: make client ARGS=\"-i ip -c count_of_players -n Name\""

build: $(EXE)
	@echo "Building project..."

server: clean build
	./$(EXE) $(ARGS)

client: clean build
	./$(EXE) $(ARGS)

debug: $(BIN) $(HEADER)
	@$(CC) $(BIN) $(DEBUG_FLAGS) $(EXE)

leap: debug
	@valgrind ./$(EXE)

$(EXE): $(BIN) $(HEADER)
	@echo "Compiling files..."
	@$(CC) $(BIN) $(FLAGS) $(EXE)

clean:
	@rm -vf $(EXE) $(LOGS)

clear:
	@rm -vf $(EXE) $(LOGS)