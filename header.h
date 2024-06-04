#ifndef __TCP_H
#define __TCP_H

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
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
#define KEY_TO_CLIENT (int)0xFFFFFFFF // * Sent key to client

// Macros for getting ms from start of frame
#define CALCULATE_TIME_DIFF_MS(start, end) \
    (((end).tv_sec - (start).tv_sec) * 1000 + ((end).tv_usec - (start).tv_usec) / 1000)

// Macros for check is need to sleep
#define TIME_TO_SLEEP_CHECK(time) (((start_message->frame_timeout / 4) < (time)) && ((start_message->frame_timeout * 3 / 4) > (time)))

// Macros for sleep and log
#define TIME_TO_SLEEP_ACTION(time)                                                                                                    \
    do                                                                                                                                \
    {                                                                                                                                 \
        sprintf(buffer, "For %lu \twait %ld ms\n", (time), start_message->frame_timeout - (time) + start_message->frame_timeout / 2); \
        log_message(buffer, names[myIndex]);                                                                                          \
        usleep((start_message->frame_timeout - (time) + start_message->frame_timeout / 2) * 1000);                                    \
    } while (0)

#define TIMESTAMP 350 // ms
#define MAGIC 0xABCDFE01
// __ Structs for TCP
typedef struct _HEADER
{
    const uint32_t magic;
    uint32_t ptype;
    uint32_t datasize;
} packet_header;

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
    uint8_t player_name[];
} __attribute__((packed)) player_send_info;

struct start
{
    uint32_t frame_timeout;
    uint32_t players_count;
    player_send_info players[];
};

// GeneratingMap.c
uint8_t *generate_quarter_map();
void validate_map(uint8_t *map);
void place_player_on_map(uint8_t *map);
void initialize_full_map(uint8_t *map);

// main.c
int parse_args(int *argc, char **argv[], uint8_t **ip, uint16_t *port, uint8_t *count, uint8_t **name);
void free_variables(uint8_t *map);

// TCP.c
int Socket(const int domain, const int type, const int protocol);
bool Setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Listen(int sockfd, int backlog);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
ssize_t Recv(int sockfd, void *buf, size_t len, int flags);
ssize_t Recv_header(int sockfd, uint32_t *datasize, uint32_t PTYPE);
ssize_t Send(int sockfd, const void *buf, size_t len, int flags);
ssize_t Send_header(int sockfd, size_t datasize, uint32_t PTYPE);
ssize_t recv_connect(int sockfd, uint8_t **buf);
ssize_t recv_map(int sockfd, uint8_t **map);
ssize_t recv_ready(int sockfd);
ssize_t recv_start(int sockfd, struct start **start_message);
ssize_t recv_client_key(int sockfd, uint8_t *key);
ssize_t recv_server_key(int sockfd, uint8_t *key, uint8_t **name);
ssize_t send_connect(int sockfd, uint8_t *name, size_t nameLen);
ssize_t send_map(int sockfd, uint8_t *map);
ssize_t send_ready(int sockfd);
ssize_t send_start(int sockfd, struct start *start_message, size_t total_size);
ssize_t send_client_key(int sockfd, uint8_t key);
ssize_t send_server_key(int sockfd, uint8_t key, uint8_t *name, uint8_t namelen);
void Exit(uint8_t *map);
void print_start_msg(struct start *start_message);
uint8_t index_of_dublicate_name(uint8_t *player_name, uint8_t player_name_len);
bool start_server(uint16_t port, uint8_t count, uint8_t *map, uint8_t *name);
bool start_client(uint8_t *ip, uint16_t port, uint8_t *name, uint8_t **map);
void log_message(char *message, char *name);

// GameplayPacman.c
void add_timeval(struct timeval *start_time);
void *get_key(void *);
void player_send_info_constructor(uint8_t *name);
void draw_map(uint8_t *map, int8_t count_of_players);
void initialize_screen();
void close_screen();
void move_players(uint8_t count_of_players);
void add_score(uint8_t count_of_players);
uint32_t start_game(uint8_t count_of_players);
uint32_t end_of_game();
bool check_collisions(uint8_t player_index, uint8_t direction, uint8_t count_of_players);
void init_game(uint8_t count_of_players, uint8_t *map, uint8_t *name);

// ! Global variables
extern uint16_t player_position;    // * Position of player x * 100 + y
extern uint8_t **full_map;          // * Map 40 x 30
extern bool done;                   // * Is game ended
extern struct start *start_message; // * Start message
extern size_t size_message;         // * Size of message
extern int fd_all[4];               // * File desctriptors

#endif
