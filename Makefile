main: main.c parser.o collect.o
	gcc -o main main.c parser.o collect.o

%.o: %.c
	gcc -c $< -o $@

clean:
	rm -f main *.o