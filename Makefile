# CNET Makefile

CC = g++
CFLAGS  = -pthread -Wall -g -stdlib=libc++

objects = Router Client DNS ProxyRouter
all: $(objects)

$(objects): %: %.cpp
	$(CC) $(CFLAGS) -o ./executable/$@ $<