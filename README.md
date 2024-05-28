<!-- TODO -->
# The Start of the Pacman project on C

## Initialization parameters
### IP
IP *for connect to server.*

### PORT
PORT *is required for both, server and client.*

### COUNT_OF_PLAYERS
COUNT_OF_PLAYERS *for server, to know how many players he need to wait.*

### NAME
NAME *is default = "Slava"*

## Packet types
**0x01** - Client Connection

**0x02** - Client Ready

**0x10** - Server send map

**0x20** - Server send start game message

**0x00** - Client send key to server

**0xffffffff** - Server send key from 1 client to others

## **Struct prototypes**
```C
// For connected users
typedef struct user_connect
{
    int fd;
    player_send_info player;
} user;

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
```

## Guide how to **build and run** the program
```bash
docker build -t pacman_mine . # create docker image
docker run --name my_docker -p 14145:14145 -d pacman_mine # create docker container and run
docker exec -it my_docker sh # get in docker container and use sh
```
```bash
    make build
    make server ARGS="-p port -c count_of_players -n Name"
    make client ARGS="-p port -i ip -n Name"
```
