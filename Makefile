CFLAGS = -g -O0

main: main.c parser.o collect.o
	gcc $(CFLAGS) -o main main.c parser.o collect.o

%.o: %.c
	gcc $(CFLAGS) -g -c $< -o $@

clean:
	rm -f main *.o