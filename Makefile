CFLAGS = -O0 -g
CC = gcc

main: main.c parser.o collect.o
	$(CC) $(CFLAGS) -o main main.c parser.o collect.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f main *.o