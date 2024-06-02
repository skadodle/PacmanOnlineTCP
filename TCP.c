#include "header.h"

typedef struct user_connect
{
    int fd;
    player_send_info player;
} user;

struct start *start_message = NULL;
size_t size_message = 0;
int fd_all[4] = {-1, -1, -1, -1};
bool isServer = false;

extern char buffer[255];

int Socket(const int domain, const int type, const int protocol)
{
    int result = socket(domain, type, protocol);
    if (result == -1)
        perror("Can't create socket!\n");

    return result;
}

bool Setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
    int result = setsockopt(sockfd, level, optname, optval, optlen);
    if (result == -1)
    {
        perror("Can't set socket options!\n");
        return false;
    }
    return true;
}

int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int result = bind(sockfd, addr, addrlen);
    if (result == -1)
        perror("Can't bind socket!\n");

    return result;
}

int Listen(int sockfd, int backlog)
{
    int result = listen(sockfd, backlog);
    if (result == -1)
        perror("Can't listen on socket!\n");

    return result;
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int result = accept(sockfd, addr, addrlen);
    if (result == -1)
        perror("Can't accept on socket!\n");

    return result;
}

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int result = connect(sockfd, addr, addrlen);
    if (result == -1)
        perror("Can't connect on socket!\n");

    return result;
}

ssize_t Recv(int sockfd, void *buf, size_t len, int flags)
{
    ssize_t size = recv(sockfd, buf, len, flags);
    if (size == -1)
        perror("Error receiving data from Vladimir Krasno Solnishko!\n");

    return size;
}

ssize_t Recv_header(int sockfd, uint32_t *datasize, uint32_t PTYPE)
{
    packet_header header;
    ssize_t size = Recv(sockfd, &header, sizeof(header), 0);
    if (size == -1 || size == 0)
        return size;

    if (ntohl(header.magic) != MAGIC)
    {
        log_message("Wrong magic -> random packet or user use incorrect protocol!", NULL);
        return -2;
    }

    if (ntohl(header.ptype) != PTYPE)
    {
        log_message("Incorrect packet type -> user use incorrect protocol!", NULL);
        return -2;
    }

    *datasize = ntohl(header.datasize);

    // printf("Correctly receive header with datasize = %d!\n", *datasize);

    return size;
}

ssize_t Send(int sockfd, const void *buf, size_t len, int flags)
{
    ssize_t size = send(sockfd, buf, len, flags);
    if (size == -1)
        perror("Error sending data to Vladimir Krasno Solnishko!\n");

    return size;
}

ssize_t Send_header(int sockfd, size_t datasize, uint32_t PTYPE)
{
    packet_header header = {htonl(MAGIC), htonl(PTYPE), htonl(datasize)};
    ssize_t size = Send(sockfd, &header, sizeof(header), 0);
    if (size == 0)
        return 0;
    if (size == -1)
    {
        log_message("Can't send the whole packet header!", NULL);
        return -1;
    }
    else if (size != sizeof(header))
    {
        log_message("Can't send the whole packet header! size != sizeof(header)", NULL);
        return -1;
    }

    return size;
}

ssize_t recv_connect(int sockfd, uint8_t **buf)
{
    uint32_t datasize = 0;
    ssize_t size = Recv_header(sockfd, &datasize, CONNECTION);
    if (size == -1 || size == -2 || size == 0)
        return size;

    (*buf) = (uint8_t *)malloc(datasize * sizeof(uint8_t));

    size = Recv(sockfd, (uint8_t *)*buf, datasize, 0);
    if (size == -1)
        return -1;

    printf("\n\tReceive connect!\n\n");

    return size;
}

ssize_t recv_map(int sockfd, uint8_t **map)
{
    uint32_t datasize = 0;
    ssize_t size = Recv_header(sockfd, &datasize, SEND_MAP);
    if (size == -1 || size == -2 || size == 0)
        return size;

    *map = (uint8_t *)malloc(sizeof(uint8_t) * datasize);
    if (map == NULL)
    {
        perror("Map malloc failure\n");
        exit(EXIT_FAILURE);
    }

    size = Recv(sockfd, (uint8_t *)(*map), datasize, 0);
    if (size == -1)
        return -1;

    printf("\n\tReceive map!\n\n");

    return size;
}

ssize_t recv_ready(int sockfd)
{
    uint32_t datasize = 0;
    ssize_t size = Recv_header(sockfd, &datasize, CLIENT_READY);
    if (size == -1 || size == -2 || size == 0)
        return size;

    if (datasize != 0)
    {
        log_message("Incorrect packet size -> user use incorrect protocol!", NULL);
        return -2;
    }

    printf("\n\tReceive ready!\n\n");

    return size;
}

ssize_t recv_start(int sockfd, struct start **start_message)
{
    uint32_t datasize = 0;
    ssize_t size = Recv_header(sockfd, &datasize, START_GAME);
    if (size == -1 || size == -2 || size == 0)
        return size;

    if (datasize == 0)
    {
        log_message("Incorrect packet size -> user use incorrect protocol!", NULL);
        return -2;
    }

    printf("\n\tReceive start message!\n\n");

    *start_message = (struct start *)malloc(datasize);

    size = Recv(sockfd, *start_message, datasize, 0);
    if (size == -1 || size == -2)
        return size;

    size_message = datasize;
    (*start_message)->frame_timeout = ntohl((*start_message)->frame_timeout);
    (*start_message)->players_count = ntohl((*start_message)->players_count);

    // Pointer to start of players
    uint8_t *ptr = (uint8_t *)(*start_message)->players;

    for (int i = 0; i < (*start_message)->players_count; i++)
    {
        player_send_info *player = (player_send_info *)ptr;
        player->start_x = ntohl(player->start_x);
        player->start_y = ntohl(player->start_y);
        player->direction = ntohl(player->direction);
        player->player_name_len = ntohl(player->player_name_len);
        // printf("Player %d: (%u, %u, %u, %u, %s)\n", i, player->start_x, player->start_y, player->direction, player->player_name_len, player->player_name);

        // Move pointer to next player
        ptr += sizeof(player_send_info) + player->player_name_len;
    }

    return 0;
}

ssize_t recv_client_key(int sockfd, uint8_t *key)
{
    uint32_t datasize = 0;
    ssize_t size = Recv_header(sockfd, &datasize, KEY_TO_SERVER);
    if (size == -1 || size == -2 || size == 0)
        return size;

    if (datasize != 1)
    {
        log_message("Incorrect packet size -> user use incorrect protocol!", NULL);
        return -2;
    }

    size = Recv(sockfd, key, 1, 0);
    if (size == -1 || size == -2)
        return size;

    return size;
}

ssize_t recv_server_key(int sockfd, uint8_t *key, uint8_t **name)
{
    uint32_t datasize = 0;
    ssize_t size = Recv_header(sockfd, &datasize, KEY_TO_CLIENT);
    if (size == -1 || size == -2 || size == 0)
        return size;

    if (datasize == 0)
    {
        log_message("Incorrect packet size -> user use incorrect protocol!", NULL);
        return -2;
    }

    size = Recv(sockfd, key, 1, 0);
    if (size == -1 || size == -2)
        return size;

    (*name) = (uint8_t *)malloc(datasize - 1);

    size = Recv(sockfd, *name, datasize - 1, 0);
    if (size == -1 || size == -2)
        return size;

    return size;
}

ssize_t send_connect(int sockfd, uint8_t *name, size_t nameLen)
{
    ssize_t size = Send_header(sockfd, nameLen, CONNECTION);
    if (size == -1 || size == 0)
        return size;

    if (name[nameLen] != '\0')
        name[nameLen] = '\0';

    size = Send(sockfd, name, nameLen, 0);
    if (size == -1 || size != nameLen)
    {
        log_message("Can't send the name!", NULL);
        return -1;
    }

    printf("\n\tSend connect!\n\n");

    return size;
}

ssize_t send_map(int sockfd, uint8_t *map)
{
    ssize_t size = Send_header(sockfd, MAP_QUARTER_HEIGHT * MAP_QUARTER_WIDTH, SEND_MAP);
    if (size == -1 || size == 0)
        return size;

    size = Send(sockfd, map, MAP_QUARTER_HEIGHT * MAP_QUARTER_WIDTH, 0);
    if (size == -1 || size != MAP_QUARTER_HEIGHT * MAP_QUARTER_WIDTH)
    {
        log_message("Can't send the map!", NULL);
        return -1;
    }

    printf("\n\tSend the map!\n\n");

    return size;
}

ssize_t send_ready(int sockfd)
{
    ssize_t size = Send_header(sockfd, 0, CLIENT_READY);
    if (size == -1 || size == 0)
        return size;

    printf("\n\tSend ready!\n\n");

    return size;
}

ssize_t send_start(int sockfd, struct start *start_message, size_t total_size)
{
    ssize_t size = Send_header(sockfd, total_size, START_GAME);
    if (size == -1 || size == 0)
        return size;

    size = Send(sockfd, start_message, total_size, 0);
    if (size == -1 || size != total_size)
    {
        log_message("Can't send the start message!", NULL);
        return -1;
    }

    printf("\n\tCorrectly send start message!\n\n");

    return 0;
}

ssize_t send_client_key(int sockfd, uint8_t key)
{
    ssize_t size = Send_header(sockfd, 1, KEY_TO_SERVER);
    if (size == -1 || size == 0)
        return size;
    size = Send(sockfd, &key, 1, 0);
    if (size == -1 || size != 1)
    {
        log_message("Can't send the key!", NULL);
        return -1;
    }

    return size;
}

ssize_t send_server_key(int sockfd, uint8_t key, uint8_t *name, uint8_t namelen)
{
    ssize_t size = Send_header(sockfd, 1 + namelen, KEY_TO_CLIENT);
    if (size == -1 || size == 0)
        return size;
    size = Send(sockfd, &key, 1, 0);
    if (size == -1 || size != 1)
    {
        log_message("Can't send the key!", NULL);
        return -1;
    }
    size = Send(sockfd, name, namelen, 0);
    sprintf(buffer, "Send server key with namelen = %d and name = %s", namelen, name);
    log_message(buffer, NULL);
    if (size == -1 || size != namelen)
    {
        log_message("Can't send the name!", NULL);
        return -1;
    }
    return 0;
}

void Exit(uint8_t *map)
{
    free_variables(map);
    exit(EXIT_FAILURE);
}

void print_start_msg(struct start *start_message)
{
    // Print data
    printf("\n\tStart message:\nFrame Timeout: %u\n", start_message->frame_timeout);
    printf("Players Count: %u\n", start_message->players_count);

    // Pointer to start of players
    uint8_t *ptr = (uint8_t *)start_message->players;

    for (int i = 0; i < start_message->players_count; i++)
    {
        player_send_info *player = (player_send_info *)ptr;
        printf("Player %d: (%u, %u, %u, %u, %s)\n", i, player->start_x, player->start_y, player->direction, player->player_name_len, player->player_name);

        // Move pointer to next player
        ptr += sizeof(player_send_info) + player->player_name_len;
    }
}

uint8_t index_of_dublicate_name(uint8_t *player_name, uint8_t player_name_len)
{
    uint8_t *ptr = (uint8_t *)start_message->players;

    for (int i = 0; i < start_message->players_count; i++)
    {
        player_send_info *player = (player_send_info *)ptr;
        if (player->player_name_len == 0)
            break;

        if (player->player_name_len == player_name_len && memcmp(player_name, player->player_name, player_name_len) == 0)
            return i;

        ptr += sizeof(player_send_info) + player->player_name_len;
    }

    return 10;
}

bool start_server(uint16_t port, uint8_t count, uint8_t *map, uint8_t *name)
{
    int server_fd = Socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
        Exit(map);
    isServer = true;
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    if (Setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == false)
        Exit(map);

    if (Bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1)
        Exit(map);

    if (Listen(server_fd, 5) == -1)
        Exit(map);

    size_t total_size = sizeof(struct start);
    size_t player_size = 0;

    start_message = (struct start *)malloc(total_size);
    if (start_message == NULL)
    {
        perror("Failed to allocate memory for start message!\n");
        Exit(map);
    }

    start_message->frame_timeout = TIMESTAMP;
    start_message->players_count = 1;

    // Add server as player
    player_size = sizeof(player_send_info) + strlen((const char *)name) + 1;
    total_size += player_size;
    start_message = (struct start *)realloc(start_message, total_size);
    if (start_message == NULL)
    {
        perror("Failed to reallocate memory for server!\n");
        Exit(map);
    }

    player_send_info *player = (player_send_info *)((uint8_t *)start_message + total_size - player_size);
    player->start_x = player_position / 100;
    player->start_y = player_position % 100;
    player->direction = RIGHT;
    player->player_name_len = strlen((const char *)name) + 1;
    memcpy(player->player_name, name, player->player_name_len);

    fd_all[0] = server_fd;

    for (uint8_t i = 1; i < count; i++)
    {
        NEW_LINE
        NEW_LINE
        user *temp_player = (struct user_connect *)malloc(sizeof(struct user_connect));
        temp_player->fd = Accept(server_fd, NULL, NULL);
        if (temp_player->fd == -1)
        {
            i--;
            close(temp_player->fd);
            continue;
        }

        uint8_t *buf = NULL;
        ssize_t size = 0;

        size = recv_connect(temp_player->fd, (uint8_t **)&buf);

        temp_player = (struct user_connect *)realloc(temp_player, sizeof(struct user_connect *) + sizeof(struct player) + size * sizeof(uint8_t));

        temp_player->player.player_name_len = size;
        memcpy(temp_player->player.player_name, buf, size);
        free(buf);
        if (temp_player->player.player_name_len == -1)
        {
            i--;
            close(temp_player->fd);
            free(temp_player);
            continue;
        }
        else if (temp_player->player.player_name_len == -2)
        {
            close(temp_player->fd);
            free(temp_player);
            continue;
        }
        if (index_of_dublicate_name(temp_player->player.player_name, temp_player->player.player_name_len) != 10)
        {
            printf("Player with name %s already exists! Abort player!\n", temp_player->player.player_name);
            i--;
            close(temp_player->fd);
            free(temp_player);
            continue;
        }

        printf("Connect Player: %s\n", temp_player->player.player_name);

        size = send_map(temp_player->fd, map);
        if (size == -1 || size != MAP_QUARTER_HEIGHT * MAP_QUARTER_WIDTH)
        {
            i--;
            log_message("Can't send map", NULL);
            close(temp_player->fd);
            free(temp_player);
            continue;
        }

        size = recv_ready(temp_player->fd);
        if (size == -1 || size == -2)
        {
            i--;
            printf("Can't recv ready\n");
            close(temp_player->fd);
            free(temp_player);
            continue;
        }

        total_size += sizeof(player_send_info) + temp_player->player.player_name_len;
        start_message->players_count++;
        start_message = (struct start *)realloc(start_message, total_size);
        if (start_message == NULL)
        {
            perror("Failed to reallocate memory for new player!\n");
            free(temp_player);
            Exit(map);
        }
        player_send_info *player = (player_send_info *)((uint8_t *)start_message + total_size - temp_player->player.player_name_len - sizeof(player_send_info));
        player->start_x = i % 2 == 1 ? (MAP_FULL_WIDTH - 1 - player_position / 100) : player_position / 100;
        player->start_y = i < 2 ? player_position % 100 : MAP_FULL_HEIGHT - 1 - player_position % 100;
        player->direction = i % 2 == 1 ? LEFT : RIGHT;
        player->player_name_len = temp_player->player.player_name_len;
        memcpy(player->player_name, temp_player->player.player_name, player->player_name_len);
        // printf("x = %d\t", player->start_x);
        // printf("y = %d\t", player->start_y);
        // printf("direction = %d\t", player->direction);
        // printf("player_name_len = %d\t", player->player_name_len);
        // printf("player_name = %s\n", player->player_name);

        fd_all[i] = temp_player->fd;

        free(temp_player);
    }

    start_message->frame_timeout = htonl(start_message->frame_timeout);
    start_message->players_count = htonl(start_message->players_count);

    // Pointer to start of players
    uint8_t *ptr = (uint8_t *)start_message->players;

    for (int i = 0; i < ntohl(start_message->players_count); i++)
    {
        player_send_info *player = (player_send_info *)ptr;
        player->start_x = htonl(player->start_x);
        player->start_y = htonl(player->start_y);
        player->direction = htonl(player->direction);
        player->player_name_len = htonl(player->player_name_len);

        // Move pointer to next player
        ptr += sizeof(player_send_info) + ntohl(player->player_name_len);
    }

    // Send start
    for (int i = 1; i < count; i++)
    {
        ssize_t size = send_start(fd_all[i], start_message, total_size);
        if (size == -1)
        {
            log_message("Can't send start", NULL);
            close(fd_all[i]);
            continue;
        }
    }

    start_message->frame_timeout = ntohl(start_message->frame_timeout);
    start_message->players_count = ntohl(start_message->players_count);

    // Pointer to start of players
    ptr = (uint8_t *)start_message->players;

    for (int i = 0; i < start_message->players_count; i++)
    {
        player_send_info *player = (player_send_info *)ptr;
        player->start_x = ntohl(player->start_x);
        player->start_y = ntohl(player->start_y);
        player->direction = ntohl(player->direction);
        player->player_name_len = ntohl(player->player_name_len);

        // Move pointer to next player
        ptr += sizeof(player_send_info) + player->player_name_len;
    }

    size_message = total_size;
    return false;
}

bool start_client(uint8_t *ip, uint16_t port, uint8_t *name, uint8_t **map)
{
    int server_fd = Socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
        exit(EXIT_FAILURE);

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr((const char *)ip);

    if (Connect(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1)
        exit(EXIT_FAILURE);

    if (send_connect(server_fd, name, strlen((const char *)name) + 1) == -1)
        exit(EXIT_FAILURE);

    ssize_t size = recv_map(server_fd, map);

    if (size == -1 || size == -2)
    {
        Exit(*map);
    }

    if (*map == NULL)
        exit(EXIT_FAILURE);

    size = send_ready(server_fd);
    if (size == -1)
        Exit(*map);

    size = recv_start(server_fd, &start_message);

    fd_all[1] = server_fd;

    return true;
}

void log_message(char *message, char *name)
{
    char filename[100];
    if (!isServer)
        sprintf(filename, "logsClient%s.txt", name);
    const char *log_filename = isServer ? "logsServer.txt" : filename;
    FILE *file = fopen(log_filename, "a"); // Открываем файл на дозапись
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    // Получаем текущее время
    time_t now;
    time(&now);
    char *time_str = ctime(&now);
    time_str[strlen(time_str) - 1] = '\0'; // Удаляем символ новой строки

    // Записываем время и сообщение в файл
    fprintf(file, "[%s] %s\n", time_str, message);
    fclose(file); // Закрываем файл
}
