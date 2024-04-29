#ifndef __TCP_H
#define __TCP_H

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <getopt.h>

// KEYS
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

// MAP PARAMETERS
#define MAP_FULL_WIDTH 40
#define MAP_FULL_HEIGHT 30

#define MAP_QUARTER_WIDTH 20
#define MAP_QUARTER_HEIGHT 15

// MAP OBJECTS
#define NEW_LINE printf("\n");
#define EMPTY 0x00 // * ' ' space
#define EMPTY_SYMBOL ' '
#define WALL 0xFF // * ' ' with blue color
#define WALL_SYMBOL ' '
#define DOT 0xAA // * Symbol to eat *
#define DOT_SYMBOL '*'
#define PLAYER 0x22 // * Player symbol @
#define PLAYER_SYMBOL '@'

// PACKET TYPES
#define CONNECTION 0x1
#define CLIENT_READY 0x2
#define SEND_MAP 0x10
#define START_GAME 0x20
#define KEY_TO_SERVER 0x0             // * Sent key to server
#define KET_TO_CLIENT (int)0xFFFFFFFF // * Sent key to client

#define TIMESTAMP 250 // ms
#define MAGIC 0xABCDFE01
// __ Structs for TCP
typedef struct _HEADER
{
    const uint32_t magic;
    uint32_t ptype;
    uint32_t datasize;
} packet_header;

/*
В C атрибут __attribute__((constructor)) используется для указания функции, которая должна быть вызвана автоматически при запуске программы, до вызова функции `main()`. Он не применяется к конструкторам структур напрямую. Однако, вы можете использовать его, чтобы вызывать ваш конструктор функцию при инициализации какой-либо глобальной переменной, которая затем будет использована как ваша структура. Вот пример:

#include <stdint.h>
#include <stdio.h>

typedef struct _HEADER
{
    const uint32_t magic;
    uint32_t ptype;
    uint32_t datasize;
} packet_header;

// Конструктор для инициализации структуры
packet_header create_packet_header(uint32_t ptype, uint32_t datasize) {
    return (packet_header){.magic = 0xABCDFE01, .ptype = ptype, .datasize = datasize};
}

// Глобальная переменная, инициализируемая конструктором
packet_header global_header __attribute__((constructor)) = create_packet_header(0, 0);

int main() {
    // Использование глобальной переменной
    printf("Magic: 0x%X, Ptype: %u, Datasize: %u\n", global_header.magic, global_header.ptype, global_header.datasize);
    return 0;
}


В этом примере функция create_packet_header вызывается при инициализации глобальной переменной `global_header`. Функция `create_packet_header` создает экземпляр структуры `packet_header` с соответствующими значениями, включая `magic`, и этот экземпляр используется как глобальная переменная.
 */

struct packet
{
    packet_header header;
    uint8_t *data;
} __attribute__((packed));

typedef struct player
{
    uint32_t start_x;
    uint32_t start_y;
    uint32_t direction;
    uint32_t player_name_len;
    uint8_t player_name[256];
} __attribute__((packed)) player_send_info;

struct msg
{
    uint32_t players_count;
    player_send_info *players;
    uint32_t frame_timeout;
};

// * Functions for generating map (GeneratingMap.c)
uint8_t *generate_quarter_map();
void validate_map(uint8_t *map);
void place_player_on_map(uint8_t *map);
void initialize_full_map(uint8_t *map);

// Functions from main.c
int parse_args(int *argc, char **argv[]);
void free_variables(uint8_t *map);

// Functions from GameplayPacman.c
void *get_key(void *);
void player_send_info_constructor(uint8_t count_of_players);
void draw_map(uint8_t *map, int8_t count_of_players);
void initialize_screen();
void close_screen();
void move_players(uint8_t count_of_players);
void add_score(uint8_t count_of_players);
uint32_t start_game(uint8_t count_of_players);
uint32_t end_of_game();
bool check_collisions(uint8_t player_index, uint8_t direction, uint8_t count_of_players);
void init_game(uint8_t count_of_players, uint8_t *map);

// ! Global variables
extern uint16_t player_position;    // * Position of player x * 100 + y
extern uint8_t **full_map;          // * Map 40 x 30
extern player_send_info players[4]; // * Info about players
extern bool done;                   // * Is game ended

#endif