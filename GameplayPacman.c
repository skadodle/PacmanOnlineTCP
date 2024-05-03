#include "header.h"

uint8_t local_direction = RIGHT;
player_send_info players[4];

static uint16_t key = 0;
static uint16_t min_x, min_y;
extern bool done;
static uint16_t score[4] = {0, 0, 0, 0};
static uint16_t total_score = 0;

void *get_key(void *)
{
    while (!done)
    {
        key = getch();
        switch (key)
        {
        case 'w':
            if (players[0].start_y != 0 && full_map[players[0].start_x][players[0].start_y - 1] != WALL)
                players[0].direction = UP;
            break;
        case KEY_UP:
            if (players[1].start_y != 0 && full_map[players[1].start_x][players[1].start_y - 1] != WALL)
                players[1].direction = UP;
            break;
        case 't':
            if (players[2].start_y != 0 && full_map[players[2].start_x][players[2].start_y - 1] != WALL)
                players[2].direction = UP;
            break;
        case 'i':
            if (players[3].start_y != 0 && full_map[players[3].start_x][players[3].start_y - 1] != WALL)
                players[3].direction = UP;
            break;

        case 'a':
            if (players[0].start_x != 0 && full_map[players[0].start_x - 1][players[0].start_y] != WALL)
                players[0].direction = LEFT;
            break;
        case KEY_LEFT:
            if (players[1].start_x != 0 && full_map[players[1].start_x - 1][players[1].start_y] != WALL)
                players[1].direction = LEFT;
            break;
        case 'f':
            if (players[2].start_x != 0 && full_map[players[2].start_x - 1][players[2].start_y] != WALL)
                players[2].direction = LEFT;
            break;
        case 'j':
            if (players[3].start_x != 0 && full_map[players[3].start_x - 1][players[3].start_y] != WALL)
                players[3].direction = LEFT;
            break;

        case 'd':
            if (players[0].start_x != 39 && full_map[players[0].start_x + 1][players[0].start_y] != WALL)
                players[0].direction = RIGHT;
            break;
        case KEY_RIGHT:
            if (players[1].start_x != 39 && full_map[players[1].start_x + 1][players[1].start_y] != WALL)
                players[1].direction = RIGHT;
            break;
        case 'h':
            if (players[2].start_x != 39 && full_map[players[2].start_x + 1][players[2].start_y] != WALL)
                players[2].direction = RIGHT;
            break;
        case 'l':
            if (players[3].start_x != 39 && full_map[players[3].start_x + 1][players[3].start_y] != WALL)
                players[3].direction = RIGHT;
            break;

        case 's':
            if (players[0].start_y != 29 && full_map[players[0].start_x][players[0].start_y + 1] != WALL)
                players[0].direction = DOWN;
            break;
        case KEY_DOWN:
            if (players[1].start_y != 29 && full_map[players[1].start_x][players[1].start_y + 1] != WALL)
                players[1].direction = DOWN;
            break;
        case 'g':
            if (players[2].start_y != 29 && full_map[players[2].start_x][players[2].start_y + 1] != WALL)
                players[2].direction = DOWN;
            break;
        case 'k':
            if (players[3].start_y != 29 && full_map[players[3].start_x][players[3].start_y + 1] != WALL)
                players[3].direction = DOWN;
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

void draw_map(uint8_t *map, int8_t count_of_players)
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

    for (uint8_t i = count_of_players; i < 4; i++)
    {
        full_map[players[i].start_x][players[i].start_y] = DOT;
    }

    init_pair(1, COLOR_BLACK, COLOR_BLUE);
    for (uint8_t x = 0; x < MAP_FULL_WIDTH; x++)
        for (uint8_t y = 0; y < MAP_FULL_HEIGHT; y++)
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
    keypad(stdscr, true);
    curs_set(0);
    start_color();
}

void close_screen()
{
    endwin();
}

void move_players(uint8_t count_of_players)
{
    for (uint8_t i = 0; i < count_of_players; i++)
        mvaddch(players[i].start_y + min_y, players[i].start_x + min_x, '@');

    refresh();

    for (uint8_t i = 0; i < count_of_players; i++)
        mvaddch(players[i].start_y + min_y, players[i].start_x + min_x, ' ');
}

void add_score(uint8_t count_of_players)
{
    for (uint8_t i = 0; i < count_of_players; i++)
    {
        score[i] += full_map[players[i].start_x][players[i].start_y] == DOT;
        full_map[players[i].start_x][players[i].start_y] = EMPTY;
    }
}

uint32_t start_game(uint8_t count_of_players)
{
    uint8_t status = 0;

    pthread_t pid;
    pthread_create(&pid, NULL, *get_key, NULL);

    while (!done)
    {
        move_players(count_of_players);
        if (total_score == score[0] + score[1] + score[2] + score[3])
        {
            done = true;
            break;
        }

        for (uint8_t i = 0; i < count_of_players; i++)
        {
            switch (players[i].direction)
            {
            case UP:
                if (players[i].start_y != 0 && full_map[players[i].start_x][players[i].start_y - 1] != WALL && check_collisions(i, players[i].direction, count_of_players))
                    players[i].start_y -= 1;
                break;
            case DOWN:
                if (players[i].start_y != 29 && full_map[players[i].start_x][players[i].start_y + 1] != WALL && check_collisions(i, players[i].direction, count_of_players))
                    players[i].start_y += 1;
                break;
            case LEFT:
                if (players[i].start_x != 0 && full_map[players[i].start_x - 1][players[i].start_y] != WALL && check_collisions(i, players[i].direction, count_of_players))
                    players[i].start_x -= 1;
                break;
            case RIGHT:
                if (players[i].start_x != 39 && full_map[players[i].start_x + 1][players[i].start_y] != WALL && check_collisions(i, players[i].direction, count_of_players))
                    players[i].start_x += 1;
                break;
            }
        }

        add_score(count_of_players);
        usleep(TIMESTAMP * 1000);
    }

    pthread_join(pid, NULL);
    return end_of_game();
}

uint32_t end_of_game()
{
    int i, j, temp;
    uint8_t indexses[4] = {0, 1, 2, 3};
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3 - i; j++)
        {
            if (score[j] > score[j + 1])
            {
                temp = score[j];
                score[j] = score[j + 1];
                score[j + 1] = temp;

                temp = indexses[j];
                indexses[j] = indexses[j + 1];
                indexses[j + 1] = temp;
            }
        }
    }

    // 1 winner
    if (score[3] > score[2])
    {
        return indexses[3];
    }
    // 2 winners
    if (score[3] == score[2] && score[2] > score[1])
    {
        if (indexses[3] > indexses[2])
            return 0xA00 + indexses[2] * 0x10 + indexses[3];
        return 0xA00 + indexses[3] * 0x10 + indexses[2];
    }
    // 3 winners
    if (score[3] == score[2] && score[2] == score[1] && score[1] > score[0])
    {
        if (indexses[3] > indexses[2] && indexses[2] > indexses[1])
            return 0xA000 + indexses[1] * 0x100 + indexses[2] * 0x10 + indexses[3];
        if (indexses[3] > indexses[1] && indexses[1] > indexses[2])
            return 0xA000 + indexses[2] * 0x100 + indexses[1] * 0x10 + indexses[3];
        if (indexses[2] > indexses[3] && indexses[3] > indexses[1])
            return 0xA000 + indexses[1] * 0x100 + indexses[3] * 0x10 + indexses[2];
        if (indexses[2] > indexses[1] && indexses[1] > indexses[3])
            return 0xA000 + indexses[3] * 0x100 + indexses[1] * 0x10 + indexses[2];
        if (indexses[1] > indexses[3] && indexses[3] > indexses[2])
            return 0xA000 + indexses[2] * 0x100 + indexses[3] * 0x10 + indexses[1];
        if (indexses[1] > indexses[2] && indexses[2] > indexses[3])
            return 0xA000 + indexses[3] * 0x100 + indexses[2] * 0x10 + indexses[1];
    }
    // TIE
    return 0xA0123;
}

bool check_collisions(uint8_t player_index, uint8_t direction, uint8_t count_of_players)
{
    uint16_t position_i = players[player_index].start_x + players[player_index].start_y * MAP_FULL_WIDTH;
    uint16_t position_j = 0;
    for (int j = 0; j < count_of_players; j++)
    {
        if (player_index == j)
            continue;
        position_j = players[j].start_y * MAP_FULL_WIDTH + players[j].start_x;
        if (direction == UP && position_i - MAP_FULL_WIDTH == position_j ||
            direction == DOWN && position_i + MAP_FULL_WIDTH == position_j ||
            direction == LEFT && position_i - 1 == position_j ||
            direction == RIGHT && position_i + 1 == position_j)
            return false;
    }
    return true;
}

void init_game(uint8_t count_of_players, uint8_t *map)
{
    player_send_info_constructor(count_of_players);

    initialize_screen();
    draw_map(map, count_of_players);

    uint32_t status = start_game(count_of_players);
    close_screen();

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
    case 0xA01:
        printf("TIE Player 0 - %s and 1 - %s is win!\n", players[0].player_name, players[1].player_name);
        break;
    case 0xA02:
        printf("TIE Player 0 - %s and 2 - %s is win!\n", players[0].player_name, players[2].player_name);
        break;
    case 0xA03:
        printf("TIE Player 0 - %s and 3 - %s is win!\n", players[0].player_name, players[3].player_name);
        break;
    case 0xA12:
        printf("TIE Player 1 - %s and 2 - %s is win!\n", players[1].player_name, players[2].player_name);
        break;
    case 0xA13:
        printf("TIE Player 1 - %s and 3 - %s is win!\n", players[1].player_name, players[3].player_name);
        break;
    case 0xA23:
        printf("TIE Player 2 - %s and 3 - %s is win!\n", players[2].player_name, players[3].player_name);
        break;
    case 0xA012:
        printf("TIE Player 0 - %s and 1 - %s and 2 - %s is win!\n", players[0].player_name, players[1].player_name, players[2].player_name);
        break;
    case 0xA013:
        printf("TIE Player 0 - %s and 1 - %s and 3 - %s is win!\n", players[0].player_name, players[1].player_name, players[3].player_name);
        break;
    case 0xA123:
        printf("TIE Player 1 - %s and 2 - %s and 3 - %s is win!\n", players[1].player_name, players[2].player_name, players[3].player_name);
        break;
    case 0xA0123:
        printf("TIE!\n");
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
}