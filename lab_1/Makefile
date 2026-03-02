CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

SRCS = main.c matrix.c algebra.c tests.c interface.c
OBJS = $(SRCS:.c=.o)
TARGET = main

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	./$(TARGET) --test

clean:
	rm -f $(OBJS) $(TARGET)