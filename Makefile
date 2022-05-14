CC=gcc

make:
	$(CC) main.c snake.c -o Snake -L /usr/lib32 -I /usr/include -lncurses

clean:
	rm ./Snake
