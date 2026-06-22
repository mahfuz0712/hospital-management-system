CC = gcc
CFLAGS = -Wall -Wextra -Isrc
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TARGET = dist/hospital.exe

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)