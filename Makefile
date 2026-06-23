# ============================================================
# Makefile - Hospital Management System
# ------------------------------------------------------------
# Usage:
#   make        -> builds dist/hospital.exe
#   make clean  -> removes compiled object files and the exe
# ============================================================

CC = gcc
CFLAGS = -Wall -Wextra -Isrc
SRC = src/main.c src/utils.c src/validation.c src/user.c src/auth.c \
      src/patient.c src/doctor.c src/appointment.c src/billing.c
TARGET = dist/hospital.exe

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean