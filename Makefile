CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lmysqlclient

TARGET = campus_sports
SRCS = main.c db.c sport_resource.c auth.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)