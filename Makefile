CC = gcc -g
FLAGS = -lncurses -lpthread -o 
DEBUG_FLAGS = -lncurses -lpthread -O0 -g -o

BIN = TCP.c main.c GenerateMap.c GameplayPacman.c
HEADER = header.h
EXE = main.exe

IP_FLAG = -i 127.0.0.1
PORT_FLAG = -p 14145
QUANTITY_FLAG = -c 2

help:
	@echo "Usage: make build"
		  "server: make server ARGS=\"-p port -c count_of_players -n Name\""
		  "client: make client ARGS=\"-i ip -c count_of_players -n Name\""

all: build
	@./$(EXE)

build: $(EXE)
	@echo "Building project..."

server: build
	./$(EXE) $(ARGS)

client: build
	./$(EXE) $(ARGS)

debug: $(BIN) $(HEADER)
	@$(CC) $(BIN) $(DEBUG_FLAGS) $(EXE)

leap: debug
	@valgrind ./$(EXE)

$(EXE): $(BIN) $(HEADER)
	@echo "Compiling files..."
	@$(CC) $(BIN) $(FLAGS) $(EXE)

clean:
	@rm -vf $(EXE)

clear:
	@rm -vf $(EXE)
