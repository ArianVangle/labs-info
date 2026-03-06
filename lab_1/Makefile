CC = gcc
CFLAGS = -Wall -Wextra -std=c2x -g 
LDFLAGS = -lm

SRCS = main.c matrix.c algebra.c tests.c interface.c
OBJS = $(SRCS:.c=.o)
TARGET = main

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	./$(TARGET) --test

clean:
	rm -f $(OBJS) $(TARGET)