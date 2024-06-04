#include "header.h"

static uint8_t x[4];
static uint8_t y[4];
static uint8_t direction[4];
static uint8_t *names[4];
static uint8_t names_len[4];

uint64_t frame_count = 0;

static uint16_t key = 0;
static uint16_t min_x, min_y;
extern bool done;
static uint16_t score[4] = {0, 0, 0, 0};
static uint16_t total_score = 0;

extern bool isServer;
static uint8_t myIndex = 0;

struct timeval start_time = {0};
struct timeval full_time = {0};

char buffer[255];

void add_timeval(struct timeval *start_time)
{
    start_time->tv_usec += start_message->frame_timeout * 1000;

    // Если количество микросекунд превысило 1 миллион, переведем их в секунды
    if (start_time->tv_usec >= 1000000)
    {
        start_time->tv_sec += start_time->tv_usec / 1000000;
        start_time->tv_usec %= 1000000;
    }
}

void *get_key(void *)
{
    struct timeval time = {0};

    fd_set read_fds;
    int max_fd = 0;

    bool isKey = false;

    for (int i = 0; i < start_message->players_count; ++i)
    {
        if (fd_all[i] > max_fd)
            max_fd = fd_all[i];
    }

    // Add stdin to the sockets
    int std_in = fileno(stdin);
    if (std_in > max_fd)
        max_fd = std_in;

    while (!done)
    {
        bool isChangedDir[4] = {false, false, false, false};
        FD_ZERO(&read_fds);

        // Добавляем стандартный ввод в набор файловых дескрипторов
        FD_SET(std_in, &read_fds);

        // Добавляем клиентские сокеты в набор файловых дескрипторов
        for (int i = 1; i < start_message->players_count; ++i)
            if (fd_all[i] != -1)
                FD_SET(fd_all[i], &read_fds);

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 1000; // 1 ms

        int activity = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);

        if (activity < 0)
        {
            perror("select");
            done = true;
            break;
        }

        // Проверяем стандартный ввод
        if (FD_ISSET(std_in, &read_fds))
        {
            key = getch();

            gettimeofday(&time, NULL);
            sprintf(buffer, "\tKEY = %d - %c  --%lu ms-- \n", (uint16_t)key, (char)key, CALCULATE_TIME_DIFF_MS(start_time, time));
            log_message((char *)buffer, (char *)names[myIndex]);
            isKey = true;
        }

        gettimeofday(&time, NULL);
        if ((TIME_TO_SLEEP_CHECK(CALCULATE_TIME_DIFF_MS(start_time, time))) && isKey)
        {
            isKey = false;
            switch (key)
            {
            case 'w':
            case KEY_UP:
                if (y[myIndex] != 0 && full_map[x[myIndex]][y[myIndex] - 1] != WALL)
                {
                    if (direction[myIndex] != UP)
                        isChangedDir[myIndex] = true;
                    direction[myIndex] = UP;
                }
                break;
            case 'a':
            case KEY_LEFT:
                if (x[myIndex] != 0 && full_map[x[myIndex] - 1][y[myIndex]] != WALL)
                {
                    if (direction[myIndex] != LEFT)
                        isChangedDir[myIndex] = true;
                    direction[myIndex] = LEFT;
                }
                break;
            case 's':
            case KEY_DOWN:
                if (y[myIndex] != 29 && full_map[x[myIndex]][y[myIndex] + 1] != WALL)
                {
                    if (direction[myIndex] != DOWN)
                        isChangedDir[myIndex] = true;
                    direction[myIndex] = DOWN;
                }
                break;
            case 'd':
            case KEY_RIGHT:
                if (x[myIndex] != 39 && full_map[x[myIndex] + 1][y[myIndex]] != WALL)
                {
                    if (direction[myIndex] != RIGHT)
                        isChangedDir[myIndex] = true;
                    direction[myIndex] = RIGHT;
                }
                break;
            case 'q':
                done = true;
                break;
            default:
                break;
            }
        }

        if (isServer)
        {
            if (isChangedDir[0])
            {
                for (uint8_t i = 1; i < start_message->players_count; i++)
                    if (fd_all[i] != -1)
                    {
                        ssize_t size = send_server_key(fd_all[i], direction[0], names[0], names_len[0]);
                        if (size == -1)
                            exit(EXIT_FAILURE);
                    }
            }

            // Проверяем клиентские сокеты
            for (int i = 1; i < start_message->players_count; ++i)
            {
                if (fd_all[i] != -1 && FD_ISSET(fd_all[i], &read_fds))
                {
                    uint8_t bufkey = 0;
                    ssize_t bytes_received = recv_client_key(fd_all[i], &bufkey);
                    sprintf(buffer, "Received client key: %d from %s\n", bufkey, names[i]);
                    log_message((char *)buffer, (char *)names[myIndex]);
                    if (bytes_received > 0)
                    {
                        switch (bufkey)
                        {
                        case UP:
                            if (y[i] != 0 && full_map[x[i]][y[i] - 1] != WALL)
                            {
                                if (direction[i] != UP)
                                    isChangedDir[i] = true;
                                direction[i] = UP;
                            }
                            break;
                        case LEFT:
                            if (x[i] != 0 && full_map[x[i] - 1][y[i]] != WALL)
                            {
                                if (direction[i] != LEFT)
                                    isChangedDir[i] = true;
                                direction[i] = LEFT;
                            }
                            break;
                        case DOWN:
                            if (y[i] != 29 && full_map[x[i]][y[i] + 1] != WALL)
                            {
                                if (direction[i] != DOWN)
                                    isChangedDir[i] = true;
                                direction[i] = DOWN;
                            }
                            break;
                        case RIGHT:
                            if (x[i] != 39 && full_map[x[i] + 1][y[i]] != WALL)
                            {
                                if (direction[i] != RIGHT)
                                    isChangedDir[i] = true;
                                direction[i] = RIGHT;
                            }
                            break;
                        default:
                            break;
                        }

                        if (isChangedDir[i])
                        {
                            for (int index = 1; index < start_message->players_count; ++index)
                            {
                                if (index != i && fd_all[index] != -1)
                                {
                                    ssize_t size = send_server_key(fd_all[index], direction[i], names[i], names_len[i]);
                                    if (size == -1)
                                        exit(EXIT_FAILURE);
                                }
                            }
                        }
                    }
                    else if (bytes_received == 0)
                    {
                        // Клиент отключился
                        close(fd_all[i]);
                        fd_all[i] = -1;
                    }
                    else if (bytes_received == -1 || bytes_received == -2)
                    {
                        exit(EXIT_FAILURE);
                    }
                }
            }
            /* Redirect direction to all players
            if (isChangedDir[1] || isChangedDir[2] || isChangedDir[3])
            {
                ssize_t size = 0;
                for (uint8_t index = 0; index < 4; index++)
                {
                    if (isChangedDir[index])
                    {
                        for (int i = 1; i < start_message->players_count; ++i)
                            if (i != index && fd_all[i] != -1)
                            {
                                size = send_server_key(fd_all[i], direction[index], names[index], names_len[index]);
                                if (size == -1)
                                {
                                    exit(EXIT_FAILURE);
                                }
                            }
                    }
                }
            }
            */
        }
        else
        {
            if (isChangedDir[myIndex])
            {
                uint8_t buf = direction[myIndex];
                ssize_t size = send_client_key(fd_all[1], buf);
                sprintf(buffer, "%s send key to server %d!", names[myIndex], buf);
                log_message(buffer, (char *)names[myIndex]);
                if (size == -1)
                {
                    close(fd_all[1]);
                    done = true;
                    break;
                }
                isChangedDir[myIndex] = false;
            }
            if (fd_all[1] != -1 && FD_ISSET(fd_all[1], &read_fds))
            {
                uint8_t key = 0;
                uint8_t *name;
                ssize_t size = recv_server_key(fd_all[1], &key, &name);
                sprintf(buffer, "Receive server key = %d from server and name = %s and size = %ld", key, name, size);
                log_message(buffer, (char *)names[myIndex]);
                if (size == 0)
                {
                    // Server disconnected
                    close(fd_all[1]);
                    fd_all[1] = -1;
                    done = true;
                    break;
                }
                if (size == -1 || size == -2)
                {
                    close(fd_all[1]);
                    for (uint8_t i = 0; i < 4; i++)
                    {
                        if (names[i] != NULL)
                            free(names[i]);
                    }
                    done = true;
                    break;
                }
                uint8_t i = index_of_dublicate_name(name, size);
                free(name);
                switch (key)
                {
                case UP:
                    if (y[i] != 0 && full_map[x[i]][y[i] - 1] != WALL)
                    {
                        if (direction[i] != UP)
                            isChangedDir[i] = true;
                        direction[i] = UP;
                    }
                    break;
                case LEFT:
                    if (x[i] != 0 && full_map[x[i] - 1][y[i]] != WALL)
                    {
                        if (direction[i] != LEFT)
                            isChangedDir[i] = true;
                        direction[i] = LEFT;
                    }
                    break;
                case DOWN:
                    if (y[i] != 29 && full_map[x[i]][y[i] + 1] != WALL)
                    {
                        if (direction[i] != DOWN)
                            isChangedDir[i] = true;
                        direction[i] = DOWN;
                    }
                    break;
                case RIGHT:
                    if (x[i] != 39 && full_map[x[i] + 1][y[i]] != WALL)
                    {
                        if (direction[i] != RIGHT)
                            isChangedDir[i] = true;
                        direction[i] = RIGHT;
                    }
                    break;
                default:
                    break;
                }
            }
        }
        // Recalculate max_fd after potentially closing sockets
        max_fd = std_in;
        for (int i = 0; i < start_message->players_count; ++i)
        {
            if (fd_all[i] > max_fd)
                max_fd = fd_all[i];
        }
    }
    return NULL;
}

void player_send_info_constructor(uint8_t *name)
{
    uint8_t *ptr = (uint8_t *)start_message->players;
    for (uint8_t i = 0; i < start_message->players_count; i++)
    {
        player_send_info *player = (player_send_info *)ptr;
        x[i] = player->start_x;
        y[i] = player->start_y;
        direction[i] = player->direction;
        names_len[i] = player->player_name_len;

        names[i] = (uint8_t *)malloc(player->player_name_len);
        memcpy(names[i], player->player_name, player->player_name_len);
        if (strcmp((const char *)names[i], (const char *)name) == 0)
            myIndex = i;

        ptr += sizeof(struct player) + player->player_name_len;
    }

    if (start_message->players_count != 4)
        for (uint8_t i = start_message->players_count; i < 4; i++)
        {
            x[i] = i % 2 == 1 ? (MAP_FULL_WIDTH - 1 - x[0]) : x[0];
            y[i] = i < 2 ? y[0] : MAP_FULL_HEIGHT - 1 - y[0];
            names_len[i] = 0;
            names[i] = NULL;
        }

    ptr = (uint8_t *)start_message->players;
    for (uint8_t i = 0; i < start_message->players_count; i++)
    {
        player_send_info *player = (player_send_info *)ptr;
        printf("(%d, %d), dir = %d, name_len = %d, name = %s\n",
               x[i], y[i], direction[i], player->player_name_len, names[i]);
        ptr += sizeof(struct player) + player->player_name_len;
    }
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
        full_map[x[i]][y[i]] = DOT;
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
        mvaddch(y[i] + min_y, x[i] + min_x, '@');

    refresh();

    for (uint8_t i = 0; i < count_of_players; i++)
        mvaddch(y[i] + min_y, x[i] + min_x, ' ');
}

void add_score(uint8_t count_of_players)
{
    for (uint8_t i = 0; i < count_of_players; i++)
    {
        score[i] += full_map[x[i]][y[i]] == DOT;
        full_map[x[i]][y[i]] = EMPTY;
    }
}

uint32_t start_game(uint8_t count_of_players)
{
    pthread_t pid;
    pthread_create(&pid, NULL, *get_key, NULL);

    struct timeval frame_time = {0};
    frame_time = start_time;
    gettimeofday(&start_time, NULL);

    sprintf(buffer, "Starting game - %lu ms\n", CALCULATE_TIME_DIFF_MS(full_time, start_time));
    log_message((char *)buffer, (char *)names[myIndex]);

    while (!done)
    {
        gettimeofday(&frame_time, NULL);
        if (((frame_time.tv_sec - start_time.tv_sec) * 1000000 + frame_time.tv_usec - start_time.tv_usec) / 1000 >= start_message->frame_timeout)
        {
            frame_count++;
            sprintf(buffer, "Full time = %lu \t took %lu ms \n", CALCULATE_TIME_DIFF_MS(full_time, frame_time), CALCULATE_TIME_DIFF_MS(start_time, frame_time));
            log_message((char *)buffer, (char *)names[myIndex]);

            add_timeval(&start_time);

            sprintf(buffer, "\t CURRENT FRAME = %lu \n", frame_count);
            log_message((char *)buffer, (char *)names[myIndex]);

            move_players(count_of_players);
            if (total_score == score[0] + score[1] + score[2] + score[3])
            {
                done = true;
                break;
            }

            for (uint8_t i = 0; i < count_of_players; i++)
            {
                switch (direction[i])
                {
                case UP:
                    if (y[i] != 0 && full_map[x[i]][y[i] - 1] != WALL && check_collisions(i, direction[i], count_of_players))
                        y[i] -= 1;
                    break;
                case DOWN:
                    if (y[i] != 29 && full_map[x[i]][y[i] + 1] != WALL && check_collisions(i, direction[i], count_of_players))
                        y[i] += 1;
                    break;
                case LEFT:
                    if (x[i] != 0 && full_map[x[i] - 1][y[i]] != WALL && check_collisions(i, direction[i], count_of_players))
                        x[i] -= 1;
                    break;
                case RIGHT:
                    if (x[i] != 39 && full_map[x[i] + 1][y[i]] != WALL && check_collisions(i, direction[i], count_of_players))
                        x[i] += 1;
                    break;
                }
            }

            add_score(count_of_players);
        }
        gettimeofday(&frame_time, NULL);
        if (((frame_time.tv_sec - start_time.tv_sec) * 1000000 + frame_time.tv_usec - start_time.tv_usec) / 1000 < start_message->frame_timeout / 50)
            usleep(start_message->frame_timeout - ((frame_time.tv_sec - start_time.tv_sec) * 1000000 + frame_time.tv_usec - start_time.tv_usec) / 1000);
        else
            usleep(start_message->frame_timeout / 50);
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
    uint16_t position_i = x[player_index] + y[player_index] * MAP_FULL_WIDTH;
    uint16_t position_j = 0;
    for (int j = 0; j < count_of_players; j++)
    {
        if (player_index == j)
            continue;
        position_j = y[j] * MAP_FULL_WIDTH + x[j];
        if ((direction == UP && position_i - MAP_FULL_WIDTH == position_j) ||
            (direction == DOWN && position_i + MAP_FULL_WIDTH == position_j) ||
            (direction == LEFT && position_i - 1 == position_j) ||
            (direction == RIGHT && position_i + 1 == position_j))
            return false;
    }
    return true;
}

void init_game(uint8_t count_of_players, uint8_t *map, uint8_t *name)
{
    player_send_info_constructor(name);

    initialize_screen();
    draw_map(map, count_of_players);

    uint32_t status = start_game(count_of_players);
    close_screen();

    switch (status)
    {
    case 0:
        printf("Player 0 - %s is win!\n", names[0]);
        break;
    case 1:
        printf("Player 1 - %s is win!\n", names[1]);
        break;
    case 2:
        printf("Player 2 - %s is win!\n", names[2]);
        break;
    case 3:
        printf("Player 3 - %s is win!\n", names[3]);
        break;
    case 0xA01:
        printf("TIE Player 0 - %s and 1 - %s is win!\n", names[0], names[1]);
        break;
    case 0xA02:
        printf("TIE Player 0 - %s and 2 - %s is win!\n", names[0], names[2]);
        break;
    case 0xA03:
        printf("TIE Player 0 - %s and 3 - %s is win!\n", names[0], names[3]);
        break;
    case 0xA12:
        printf("TIE Player 1 - %s and 2 - %s is win!\n", names[1], names[2]);
        break;
    case 0xA13:
        printf("TIE Player 1 - %s and 3 - %s is win!\n", names[1], names[3]);
        break;
    case 0xA23:
        printf("TIE Player 2 - %s and 3 - %s is win!\n", names[2], names[3]);
        break;
    case 0xA012:
        printf("TIE Player 0 - %s and 1 - %s and 2 - %s is win!\n", names[0], names[1], names[2]);
        break;
    case 0xA013:
        printf("TIE Player 0 - %s and 1 - %s and 3 - %s is win!\n", names[0], names[1], names[3]);
        break;
    case 0xA123:
        printf("TIE Player 1 - %s and 2 - %s and 3 - %s is win!\n", names[1], names[2], names[3]);
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

    printf("\n\t Count of frames: %lu\n\n", frame_count);

    for (uint8_t i = 0; i < start_message->players_count; i++)
        free(names[i]);
}
