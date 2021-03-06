# default is CC = cc
CC = gcc
CFLAGS = -g -Wall -fsanitize=address -O1 -fno-omit-frame-pointer
CPPFLAGS =          # default is CPPFLAGS = [blank]
LDFLAGS = -lpthread # default is LDFLAGS = [blank]

# default compile command: $(CC) $(CFLAGS) $(CPPFLAGS) -c -o <foo>.o <foo>.c

all: clean ioring

ioring: ioring.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)
# default linking command: $(CC) $(LDFLAGS) <foo>.o -o <foo>

clean:
	$(RM) -f *.o ioring
