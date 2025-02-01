CC = gcc
CFLAGS = -I src/include
LDFLAGS = -L src/lib -lSDL2 -lmingw32 -lm -static-libgcc
SOURCES = src/main.c
OUTPUT = falling-sand

all: build run clean

build:
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SOURCES) $(LDFLAGS)

run:
	./$(OUTPUT)

clean:
	del $(OUTPUT).exe
