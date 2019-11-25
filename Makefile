main: main.c parser.o collect.o
	gcc -g -o main main.c parser.o collect.o



%.o: %.c
	gcc -g -c $< -o $@

clean:
	rm -f main *.o