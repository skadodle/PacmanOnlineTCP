#include "header.h"

bool done = false;
// DELETE START TIME
extern struct timeval full_time;

int parse_args(int *argc, char **argv[], uint8_t **ip, uint16_t *port, uint8_t *count, uint8_t **name)
{
    int r = 0;
    uint8_t cot = 0;
    while ((r = getopt(*argc, *argv, "i:p:c:n:")) != -1)
    {
        switch (r)
        {
        case 'i':
            (*ip) = (uint8_t *)optarg;
            cot++;
            break;
        case 'p':
            if (atoi(optarg) <= 0)
            {
                printf("Invalid port: port is negative!\n");
                return 1;
            }
            (*port) = atoi(optarg);
            cot++;
            break;
        case 'c':
            (*count) = atoi(optarg);
            cot++;
            break;
        case 'n':
            (*name) = (uint8_t *)optarg;
            cot++;
            break;
        default:
            return 1;
        }
    }
    return cot == 0 ? 1 : 0;
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
    uint16_t port = 0;
    uint8_t *ip = NULL;
    uint8_t count = 0;
    uint8_t *name = (uint8_t *)"Slava\0";

    uint8_t *map = NULL;

    if (parse_args(&argc, &argv, &ip, &port, &count, &name) == 1)
    {
        printf("Usage: -p port [-c count (for server)] [-i ip (for client)] [-n name (\" Slava \" is default)].\n");
        return 1;
    }

    if (ip == NULL && count == 0)
    {
        printf("Invalid arguments! Don't use flags -i and -c together!\n");
        return 1;
    }

    if (count != 0 && (count < 2 || count > 4))
    {
        printf("COUNT of players = %d\n", count);
        printf("Count of players must be [2 - 4]!\n");
        return 1;
    }

    if (ip == NULL)
    {
        map = generate_quarter_map();
        start_server(port, count, map, name);
    }
    else
    {
        start_client(ip, port, name, &map);
        initialize_full_map(map);
    }

    print_start_msg(start_message);

    NEW_LINE
    NEW_LINE

    gettimeofday(&full_time, NULL);

    if (map != NULL)
        init_game(start_message->players_count, map, name);

    free(start_message);
    free_variables(map);
    return 0;
}