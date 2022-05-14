#ifndef __NCURSES_H__
#define __NCURSES_H__
#include <ncurses.h>
#endif

#ifndef __TIME_H__
#define __TIME_H__
#include <time.h>
#endif

#ifndef __STDLIB_H__
#define __STDLIB_H__
#include <stdlib.h>
#endif

#ifndef __SNAKE_H__
#define __SNAKE_H__
#include "snake.h"
#endif

int g_game_speed = 100;

void game_loop(struct Queue* q, int screenWidth, int screenHeight);

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

	srand((unsigned int) time(NULL));//seed the random number generator for food placement
	game_loop(snake, width, height);

	endwin();
	return 0;
}


void game_loop(struct Queue* q, int screenWidth, int screenHeight){
	int ch;
	bool foodPlaced = false;
	
	//start moving right by default
	int moveX = 1;
	int moveY = 0;
	char headChar = '>';
	enum direction{UP, DOWN, LEFT, RIGHT};
	enum direction blocked = LEFT;

	//players can move with arrow keys, wasd, or vi movement controls
	while((ch = getch()) != KEY_F(1)){
		//determine direction of movement
		//players can move with arrow keys, wasd, or vi movement controls
		switch(ch){
			case 'l':
			case 'd':
			case KEY_RIGHT:
				if(blocked != RIGHT){
					moveX = 1;
					moveY = 0;
					blocked = LEFT;
					headChar = '>';
				}
				break;
			case 'h':
			case 'a':
			case KEY_LEFT:
				if(blocked != LEFT){
					moveX = -1;
					moveY = 0;
					blocked = RIGHT;
					headChar = '<';
				}
				break;
			case 'k':
			case 'w':
			case KEY_UP:
				if(blocked != UP){
					moveX = 0;
					moveY = -1;
					blocked = DOWN;
					headChar = '^';
				}
				break;
			case 'j':
			case 's':
			case KEY_DOWN:
				if(blocked != DOWN){
					moveX = 0;
					moveY = 1;
					blocked = UP;
					headChar = 'v';
				}
				break;
		}

		if (!foodPlaced){
			//todo: make food
			int foodX = rand() % screenWidth;
			int foodY = rand() % screenHeight;
			mvaddch(foodY, foodX, '%');
			foodPlaced = true;
		}
		
		//move
		Segment* newSnakeHead = create_segment(q->tail->y + moveY, q->tail->x + moveX);
		//replace old head char with body char
		mvaddch(q->tail->y, q->tail->x, ACS_DIAMOND);
		
		//check new head for collision
		char atPos = mvinch(newSnakeHead->y, newSnakeHead->x) & A_CHARTEXT;
		if (atPos == ACS_DIAMOND){
			//fucking die
			moveX = 0;
			moveY = 0;
			refresh();
			return;
		}
		else if (atPos == '%'){
			//food
			enqueue(q, newSnakeHead);
			mvaddch(newSnakeHead->y, newSnakeHead->x, headChar);
			foodPlaced = false;

		}
		else {
			enqueue(q, newSnakeHead);
			mvaddch(newSnakeHead->y, newSnakeHead->x, headChar);
			//replace tail with space
			Segment* oldSnakeTail = dequeue(q);
			mvaddch(oldSnakeTail->y, oldSnakeTail->x, ' ');
			free(oldSnakeTail);
		}

		refresh();
	}

}
