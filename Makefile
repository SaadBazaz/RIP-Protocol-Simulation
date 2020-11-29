# CNET Makefile

CC = g++
CFLAGS  = -pthread -Wall -g

objects = Router Client
all: $(objects)

$(objects): %: %.cpp
	$(CC) $(CFLAGS) -o $@ $<