# CNET Makefile

CC = g++
CFLAGS  = -pthread

objects = Router Client DNS ProxyRouter
all: $(objects)

$(objects): %: %.cpp
	$(CC) $(CFLAGS) -o ./$@ $<
clean:
	rm -rf $(objects)
