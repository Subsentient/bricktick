CFLAGS = -std=gnu89 -pedantic -Wall -g3 -O0
LDFLAGS = -lcurses
CC = gcc
OUTFILE = ../bricktick

all:
	$(CC) $(CFLAGS) -c ball.c
	$(CC) $(CFLAGS) -c paddle.c
	$(CC) $(CFLAGS) -c bricks.c
	$(CC) $(CFLAGS) -c charms.c
	$(CC) $(CFLAGS) -c main.c
	
	$(CC) $(CFLAGS) -o $(OUTFILE) bricks.o ball.o paddle.o charms.o main.o $(LDFLAGS)

clean:
	rm -vf *.o *.gch
