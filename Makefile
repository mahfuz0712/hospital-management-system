CC = gcc
WINDRES = windres

CFLAGS = -Wall -Wextra -Isrc

SRC = src/main.c \
      src/utils.c \
      src/validation.c \
      src/user.c \
      src/auth.c \
      src/patient.c \
      src/doctor.c \
      src/appointment.c \
      src/billing.c

TARGET = dist/hospital.exe
RES = dist/resource.o

all: $(TARGET)

$(RES): resource.rc icon.ico
	@mkdir -p dist
	$(WINDRES) resource.rc -O coff -o $(RES)

$(TARGET): $(SRC) $(RES)
	@mkdir -p dist
	$(CC) $(CFLAGS) $(SRC) $(RES) -o $(TARGET)

clean:
	rm -f $(TARGET) $(RES)

.PHONY: all clean