#ifndef __NCURSES_H__
#define __NCURSES_H__
#include <ncurses.h>
#endif

#ifndef __STDLIB_H__
#define __STDLIB_H__
#include <stdlib.h>
#endif

#ifndef __SNAKE_H__
#define __SNAKE_H__
#include "snake.h"
#endif

#define FOOD_INTERVAL 1000

int g_game_speed = 100;

void game_loop(struct Queue* q);

int main(int argc, char* argv[]){
	
	initscr();
	timeout(g_game_speed);
	start_color();
	curs_set(0);//hide cursor
	cbreak();//disable line buffering
	keypad(stdscr, true);
	noecho();

	init_pair(1, COLOR_CYAN, COLOR_BLACK);
	attron(COLOR_PAIR(1));
	printw("Press F1 to exit");
	refresh();

	int height = getmaxy(stdscr);
	int width = getmaxx(stdscr);
	int startY = height/2;
	int startX = width/2;

	Queue* snake = initial_snake(startY, startX);
	//draw the initial snake
	Segment* sp = snake->head;
	while (sp->next != NULL){
		mvaddch(sp->y, sp->x, ACS_DIAMOND);
		sp = sp->next;
	}
	mvaddch(sp->y, sp->x, '>');

	refresh();

	game_loop(snake);

	endwin();
	return 0;
}

void game_loop(struct Queue* q){
	int ch;
	int counter = 0;
	
	//start moving right by default
	int moveX = 1;
	int moveY = 0;
	char headChar = '>';

	//players can move with arrow keys, wasd, or vi movement controls
	while((ch = getch()) != KEY_F(1)){
		//determine direction of movement
		//players can move with arrow keys, wasd, or vi movement controls
		switch(ch){
			case 'l':
			case 'd':
			case KEY_RIGHT:
				moveX = 1;
				moveY = 0;
				break;
			case 'h':
			case 'a':
			case KEY_LEFT:
				moveX = -1;
				moveY = 0;
				break;
			case 'k':
			case 'w':
			case KEY_UP:
				moveX = 0;
				moveY = -1;
				break;
			case 'j':
			case 's':
			case KEY_DOWN:
				moveX = 0;
				moveY = 1;
				break;
		}

		counter++;
		if (counter == FOOD_INTERVAL){
			counter = 0;
			//todo: make food
		}
		
		//move
		Segment* newSnakeHead = create_segment(q->tail->y + moveY, q->tail->x + moveX);
		//replace old head char with body char
		mvaddch(q->tail->y, q->tail->x, ACS_DIAMOND);
		//replace tail with space
		Segment* oldSnakeTail = dequeue(q);
		mvaddch(oldSnakeTail->y, oldSnakeTail->x, ' ');
		free(oldSnakeTail);
		//todo: check new head for collision
		enqueue(q, newSnakeHead);
		mvaddch(newSnakeHead->y, newSnakeHead->x, headChar);

		refresh();
	}

}
