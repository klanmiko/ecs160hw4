CFLAGS = -O0 -g

main: main.c parser.o collect.o
	gcc $(CFLAGS) -o main main.c parser.o collect.o

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f main *.o