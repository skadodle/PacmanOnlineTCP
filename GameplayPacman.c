#include "header.h"

// TODO game realization and handling game logic
uint8_t local_direction = RIGHT;
player_send_info players[4];

static uint16_t key = 0;
static uint16_t min_x, min_y;
extern bool done;
static uint16_t score[4] = {0, 0, 0, 0};
static uint16_t total_score = 0;

void *get_key(void *)
{
    keypad(stdscr, true);
    while (!done)
    {
        key = getch();
        switch (key)
        {
        case 'w':
        case KEY_UP:
            key = UP;
            break;

        case 'a':
        case KEY_LEFT:
            key = LEFT;
            break;

        case 'd':
        case KEY_RIGHT:
            key = RIGHT;
            break;

        case 's':
        case KEY_DOWN:
            key = DOWN;
            break;

        case 'q':
            key = 'q';
            done = true;
            break;

        default:
            break;
        }
    }
}

void player_send_info_constructor(uint8_t count_of_players)
{
    // Initialize player 0
    players[0].start_x = player_position / 100;
    players[0].start_y = player_position % 100;
    players[0].direction = local_direction;
    players[0].player_name_len = 5;
    strncpy(players[0].player_name, "Slava", players[0].player_name_len);
    players[0].player_name[strlen("Slava")] = '\0'; // Ensure null-terminated

    count_of_players--;

    // Initialize player 1
    players[1].start_x = MAP_FULL_WIDTH - 1 - player_position / 100;
    players[1].start_y = player_position % 100;
    players[1].direction = LEFT;
    players[1].player_name_len = 0; // TODO with count_of_players
    if (count_of_players <= 0)
        memset(players[1].player_name, 0, 256); // Ensure name is empty
    else
    {
        // TODO
    }
    players[1].player_name[0] = '\0';

    count_of_players--;

    // Initialize player 2
    players[2].start_x = player_position / 100;
    players[2].start_y = MAP_FULL_HEIGHT - 1 - player_position % 100;
    players[2].direction = RIGHT;
    players[2].player_name_len = 0;
    if (count_of_players <= 0)
        memset(players[2].player_name, 0, 256); // Ensure name is empty
    else
    {
        // TODO
    }
    players[2].player_name[0] = '\0';

    count_of_players--;

    // Initialize player 3
    players[3].start_x = MAP_FULL_WIDTH - 1 - player_position / 100;
    players[3].start_y = MAP_FULL_HEIGHT - 1 - player_position % 100;
    players[3].direction = LEFT;
    players[3].player_name_len = 0;
    if (count_of_players <= 0)
        memset(players[3].player_name, 0, 256); // Ensure name is empty
    else
    {
        // TODO
    }
    players[3].player_name[0] = '\0';
}

void draw_map(uint8_t *map)
{
    uint16_t max_x = 0, max_y = 0;

    getmaxyx(stdscr, max_y, max_x);

    if (max_x < MAP_QUARTER_WIDTH * 2 || max_y < MAP_QUARTER_HEIGHT * 2)
    {
        clear();
        endwin();
        free_variables(map);
        printf("Терминал слишком мал (%d : %d) для отображения поля. Пожалуйста, увеличьте размер терминала.\n", max_x, max_y);
        exit(0);
    }

    min_x = max_x / 2 - MAP_QUARTER_WIDTH;
    min_y = max_y / 2 - MAP_QUARTER_HEIGHT;

    mvaddch(min_y - 1, min_x - 1, ACS_ULCORNER);                                          // Left Upper corner
    mvaddch(min_y - 1, min_x + MAP_QUARTER_WIDTH * 2, ACS_URCORNER);                      // Right Upper corner
    mvaddch(min_y + MAP_QUARTER_HEIGHT * 2, min_x - 1, ACS_LLCORNER);                     // Left Bottom corner
    mvaddch(min_y + MAP_QUARTER_HEIGHT * 2, min_x + MAP_QUARTER_WIDTH * 2, ACS_LRCORNER); // Right Bottom corner

    mvhline(min_y - 1, min_x, ACS_HLINE, MAP_QUARTER_WIDTH * 2);                      // Upper line
    mvhline(min_y + MAP_QUARTER_HEIGHT * 2, min_x, ACS_HLINE, MAP_QUARTER_WIDTH * 2); // Bottom line
    mvvline(min_y, min_x - 1, ACS_VLINE, MAP_QUARTER_HEIGHT * 2);                     // Left line
    mvvline(min_y, min_x + MAP_QUARTER_WIDTH * 2, ACS_VLINE, MAP_QUARTER_HEIGHT * 2); // Right line
    refresh();

    init_pair(1, COLOR_BLACK, COLOR_BLUE);
    for (uint16_t x = 0; x < MAP_FULL_WIDTH; x++)
        for (uint16_t y = 0; y < MAP_FULL_HEIGHT; y++)
        {
            if (full_map[x][y] == WALL)
            {
                attron(COLOR_PAIR(1));
                mvaddch(min_y + y, min_x + x, WALL_SYMBOL);
                attroff(COLOR_PAIR(1));
            }
            else if (full_map[x][y] == DOT)
            {
                mvaddch(min_y + y, min_x + x, DOT_SYMBOL);
                total_score++;
                continue;
            }
            else if (full_map[x][y] == PLAYER)
            {
                mvaddch(min_y + y, min_x + x, PLAYER_SYMBOL);
            }
        }
    refresh();

    return;
}

void initialize_screen()
{
    // Initialize screen
    initscr();
    noecho();
    curs_set(0);
    start_color();
}

void close_screen()
{
    endwin();
}

uint8_t start_game(uint8_t count_of_players)
{
    uint8_t status = 0;

    pthread_t pid;
    pthread_create(&pid, NULL, *get_key, NULL);

    key = players[0].direction;

    while (!done)
    {
        mvaddch(players[0].start_y + min_y, players[0].start_x + min_x, '@');
        refresh();
        mvaddch(players[0].start_y + min_y, players[0].start_x + min_x, ' ');
        if (total_score == score[0] + score[1] + score[2] + score[3])
        {
            done = true;
            break;
        }
        switch (key)
        {
        case UP:
            if (players[0].start_y != 0 && full_map[players[0].start_x][players[0].start_y - 1] != WALL && check_collisions(0))
            {
                players[0].start_y -= 1;
            }
            break;
        case DOWN:
            if (players[0].start_y != 29 && full_map[players[0].start_x][players[0].start_y + 1] != WALL && check_collisions(0))
            {
                players[0].start_y += 1;
            }
            break;
        case LEFT:
            if (players[0].start_x != 0 && full_map[players[0].start_x - 1][players[0].start_y] != WALL && check_collisions(0))
            {
                players[0].start_x -= 1;
            }
            break;
        case RIGHT:
            if (players[0].start_x != 39 && full_map[players[0].start_x + 1][players[0].start_y] != WALL && check_collisions(0))
            {
                players[0].start_x += 1;
            }
            break;
        }

        score[0] += full_map[players[0].start_x][players[0].start_y] == DOT;
        full_map[players[0].start_x][players[0].start_y] = EMPTY;

        usleep(TIMESTAMP * 1000);
    }

    pthread_join(pid, NULL);
}

bool check_collisions(uint8_t player_index)
{
    uint16_t position_i = players[player_index].start_x + players[player_index].start_y * MAP_FULL_WIDTH;
    uint16_t position_j = 0;
    for (int j = 0; j < 4; j++)
    {
        if (player_index == j)
            continue;
        position_j = players[j].start_y * MAP_FULL_WIDTH + players[j].start_x;
        if (key == UP && position_i - MAP_FULL_WIDTH == position_j || key == DOWN && position_i + MAP_FULL_WIDTH == position_j || key == LEFT && position_i - 1 == position_j || key == RIGHT && position_i + 1 == position_j)
            return false;
    }
    return true;
}

void init_game(uint8_t count_of_players, uint8_t *map)
{
    player_send_info_constructor(count_of_players);

    initialize_screen();
    draw_map(map);

    int status = start_game(count_of_players);

    switch (status)
    {
    case 0:
        printf("Player 0 - %s is win!\n", players[0].player_name);
        break;
    case 1:
        printf("Player 1 - %s is win!\n", players[1].player_name);
        break;
    case 2:
        printf("Player 2 - %s is win!\n", players[2].player_name);
        break;
    case 3:
        printf("Player 3 - %s is win!\n", players[3].player_name);
        break;
    case 0xB0:
        printf("Player 0 end game!\n");
        break;
    case 0xB1:
        printf("Player 1 end game!\n");
        break;
    case 0xB2:
        printf("Player 2 end game!\n");
        break;
    case 0xB3:
        printf("Player 3 end game!\n");
        break;
    }

    close_screen();

    printf("Score for player 0: %d\n", score[0]);

    // TODO
}