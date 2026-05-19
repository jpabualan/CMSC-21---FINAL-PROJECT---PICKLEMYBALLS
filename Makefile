# --- Compiler Settings ---
CC = gcc
CFLAGS = -O1 -Wall -std=c99 -Wno-missing-braces -I include/ -I src/ -I src/UI/ -I src/core/

# --- Libraries ---
# For Windows (MinGW):
LDFLAGS = -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
# For Linux (Uncomment the line below and comment out the Windows line if on Linux):
# LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# --- Source Files ---
# This automatically grabs main.c and EVERY .c file inside your subfolders
SRC = main.c $(wildcard src/core/*.c) $(wildcard src/UI/*.c)

# --- Output Name ---
OUT = pickleball_tracker

# --- Build Rules ---
# --- Must use tab indentation beneath all and clean as it would dish out a *** missing operator ---
all:
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(OUT) $(OUT).exe