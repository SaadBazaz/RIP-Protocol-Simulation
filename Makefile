# CNET Makefile

CC = g++
CFLAGS  = -pthread -Wall -g -stdlib=libc++

objects = Router Client DNS
all: $(objects)

$(objects): %: %.cpp
	$(CC) $(CFLAGS) -o ./executable/$@ $<