#include "header.h"

uint16_t player_position = 0; // x * 100 + y
uint8_t **full_map = NULL;

uint8_t *generate_quarter_map()
{
    uint8_t *map = (uint8_t *)malloc(sizeof(uint8_t) * MAP_QUARTER_HEIGHT * MAP_QUARTER_WIDTH);
    if (map == NULL)
    {
        perror("Can't allocate memory for map!\n");
        exit(EXIT_FAILURE);
    }

    for (uint16_t i = 0; i < MAP_QUARTER_HEIGHT * MAP_QUARTER_WIDTH; i++)
    {
        map[i] = DOT;
    }

    map[(rand() % (MAP_QUARTER_HEIGHT - 2) + 1) * MAP_QUARTER_WIDTH + (rand() % (MAP_QUARTER_WIDTH - 2) + 1)] = WALL;

    uint8_t count_block = 0;

    bool flag = true;
    while (flag)
    {
        flag = false;
        for (uint16_t x = 1; x < MAP_QUARTER_WIDTH - 1; x++)
            for (uint16_t y = 1; y < MAP_QUARTER_HEIGHT - 1; y++)
            {
                if (map[y * MAP_QUARTER_WIDTH + x] == WALL)
                    continue;

                count_block = 0;

                if (map[(y - 1) * MAP_QUARTER_WIDTH + x] == WALL)
                    count_block++;
                if (map[(y + 1) * MAP_QUARTER_WIDTH + x] == WALL)
                    count_block++;
                if (map[y * MAP_QUARTER_WIDTH + (x - 1)] == WALL)
                    count_block++;
                if (map[y * MAP_QUARTER_WIDTH + (x + 1)] == WALL)
                    count_block++;
                if (map[(y + 1) * MAP_QUARTER_WIDTH + (x + 1)] == WALL)
                    count_block++;
                if (map[(y - 1) * MAP_QUARTER_WIDTH + (x + 1)] == WALL)
                    count_block++;
                if (map[(y + 1) * MAP_QUARTER_WIDTH + (x - 1)] == WALL)
                    count_block++;
                if (map[(y - 1) * MAP_QUARTER_WIDTH + (x - 1)] == WALL)
                    count_block++;

                if (count_block == 1 && rand() % 2 != 1)
                {
                    flag = true;
                    map[y * MAP_QUARTER_WIDTH + x] = WALL;
                }
            }
    }

    for (short l = 0; l < 2; l++)
        for (short i = 1; i < MAP_QUARTER_HEIGHT - 1; i++)
            for (short j = 1; j < MAP_QUARTER_WIDTH - 1; j++)
            {
                if (map[i * MAP_QUARTER_WIDTH + j] == WALL && map[(i - 1) * MAP_QUARTER_WIDTH + j] == DOT &&
                    map[(i + 1) * MAP_QUARTER_WIDTH + j] == DOT && map[i * MAP_QUARTER_WIDTH + (j - 1)] == DOT &&
                    map[i * MAP_QUARTER_WIDTH + (j + 1)] == DOT)
                {
                    map[i * MAP_QUARTER_WIDTH + j] = DOT;
                    continue;
                }
                if (map[(i + 1) * MAP_QUARTER_WIDTH + j + 1] == DOT && map[(i + 1) * MAP_QUARTER_WIDTH + j - 1] == DOT &&
                    map[(i - 1) * MAP_QUARTER_WIDTH + j + 1] == DOT && map[(i - 1) * MAP_QUARTER_WIDTH + j - 1] == DOT)
                {

                    if (map[(i - 1) * MAP_QUARTER_WIDTH + j] == WALL && map[(i + 1) * MAP_QUARTER_WIDTH + j] == DOT)
                    {
                        if (map[i * MAP_QUARTER_WIDTH + j + 1] == WALL && map[i * MAP_QUARTER_WIDTH + j - 1] == DOT)
                        {
                            if (rand() % 2 != 0)
                                map[i * MAP_QUARTER_WIDTH + j] = WALL;
                            continue;
                        }

                        if (map[i * MAP_QUARTER_WIDTH + j - 1] == WALL && map[i * MAP_QUARTER_WIDTH + j + 1] == DOT)
                        {
                            if (rand() % 2 != 0)
                                map[i * MAP_QUARTER_WIDTH + j] = WALL;
                            continue;
                        }
                    }

                    if (map[(i + 1) * MAP_QUARTER_WIDTH + j] == WALL && map[(i - 1) * MAP_QUARTER_WIDTH + j] == DOT)
                    {
                        if (map[i * MAP_QUARTER_WIDTH + j + 1] == WALL && map[i * MAP_QUARTER_WIDTH + j - 1] == DOT)
                        {
                            if (rand() % 2 != 0)
                                map[i * MAP_QUARTER_WIDTH + j] = WALL;
                            continue;
                        }

                        if (map[i * MAP_QUARTER_WIDTH + j - 1] == WALL && map[i * MAP_QUARTER_WIDTH + j + 1] == DOT)
                        {
                            if (rand() % 2 != 0)
                                map[i * MAP_QUARTER_WIDTH + j] = WALL;
                            continue;
                        }
                    }
                }
            }

    validate_map(map);
    place_player_on_map(map);
    initialize_full_map(map);

    return map;
}

void validate_map(uint8_t *map)
{
    bool flag = true;
    while (flag)
    {
        flag = false;
        for (uint8_t i = 1; i < MAP_QUARTER_HEIGHT - 1; i++)
            for (uint8_t j = 1; j < MAP_QUARTER_WIDTH - 1; j++)
            {
                uint32_t index = i * MAP_QUARTER_WIDTH + j;
                if (map[index] == DOT)
                {
                    uint8_t count = 0;
                    if (map[index - MAP_QUARTER_WIDTH] == DOT)
                        count++;
                    if (map[index + MAP_QUARTER_WIDTH] == DOT)
                        count++;
                    if (map[index + 1] == DOT)
                        count++;
                    if (map[index - 1] == DOT)
                        count++;

                    if (count < 2)
                    {
                        map[index] = WALL;
                        flag = true;
                    }
                }
            }
    }
}

void place_player_on_map(uint8_t *map)
{
    uint8_t x = rand() % MAP_QUARTER_WIDTH;
    uint8_t y = rand() % MAP_QUARTER_HEIGHT;

    while (true)
    {
        if (map[x + y * MAP_QUARTER_WIDTH] == DOT)
        {
            map[x + y * MAP_QUARTER_WIDTH] = PLAYER;
            player_position = x * 100 + y;
            break;
        }
        x = rand() % MAP_QUARTER_WIDTH;
        y = rand() % MAP_QUARTER_HEIGHT;
    }
}

void initialize_full_map(uint8_t *map)
{
    // Create Double Array for full map
    full_map = (uint8_t **)malloc(sizeof(uint8_t *) * MAP_FULL_WIDTH);
    if (full_map == NULL)
    {
        perror("Can't allocate memory for full map!\n");
        Exit(map);
    }
    for (uint8_t i = 0; i < MAP_FULL_WIDTH; i++)
    {
        full_map[i] = (uint8_t *)malloc(sizeof(uint8_t) * MAP_FULL_HEIGHT);
        for (uint8_t j = 0; j < MAP_FULL_HEIGHT; j++)
        {
            full_map[i][j] = '0';
        }
    }

    // Initialize first and second quarters
    for (uint8_t x = 0; x < MAP_QUARTER_WIDTH; x++)
    {
        for (uint8_t y = 0; y < MAP_QUARTER_HEIGHT; y++)
        {
            full_map[x][y] = map[y * MAP_QUARTER_WIDTH + x];
            full_map[x][MAP_FULL_HEIGHT - y - 1] = map[y * MAP_QUARTER_WIDTH + x];
            full_map[MAP_FULL_WIDTH - x - 1][y] = map[y * MAP_QUARTER_WIDTH + x];
            full_map[MAP_FULL_WIDTH - x - 1][MAP_FULL_HEIGHT - y - 1] = map[y * MAP_QUARTER_WIDTH + x];
        }
    }
}