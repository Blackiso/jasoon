CC = gcc
CFLAGS = -Wall -g

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
TARGET = jasoon

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(CFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJ)
