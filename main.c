#include "header.h"

uint32_t port = 0;
uint8_t *ip = NULL;
bool dev = false;
bool done = false;

int parse_args(int *argc, char **argv[])
{
    int r = 0;
    while ((r = getopt(*argc, *argv, "i:p:d")) != -1)
    {
        switch (r)
        {
        case 'i':
            ip = optarg;
            break;
        case 'p':
            port = atoi(optarg);
            break;
        case 'd':
            dev = true;
            break;
        default:
            return 1;
        }
    }
    return 0;
}

void free_variables(uint8_t *map)
{
    free(map);
    map = NULL;
    if (full_map != NULL)
    {
        for (int i = 0; i < MAP_FULL_WIDTH; i++)
        {
            free(full_map[i]);
            full_map[i] = NULL;
        }
        free(full_map);
        full_map = NULL;
    }
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    if (parse_args(&argc, &argv) == 1)
    {
        perror("Usage: -i ip -p port.\n");
        if (!dev)
            return 1;
    }

    uint8_t count_of_players = 0;
    scanf("%hhu", &count_of_players);

    uint8_t *map = generate_quarter_map();

    if (count_of_players <= 0 || count_of_players > 4)
    {
        printf("\t%d\n", count_of_players);
        perror("Count of players must be [1 - 4]!\n");
        free_variables(map);
        return 1;
    }

    init_game(count_of_players, map);

    free_variables(map);
    return 0;
}