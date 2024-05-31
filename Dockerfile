# Ипользуем официальный образ Alpine Linux
FROM alpine:latest

# Устанавливаем рабочую директорию в контейнере
WORKDIR /app

# Устанавливаем необходимые пакеты, включая GCC, make и ncurses
RUN apk add --no-cache gcc musl-dev make ncurses-dev

# Копируем все .c и .h файлы из текущей директории в рабочую директорию контейнера
COPY Makefile *.c *.h .

# Устанавливаем команду по умолчанию для выполнения при запуске контейнера
CMD ["sh", "-c", "while true; do sleep 3600; done"]
