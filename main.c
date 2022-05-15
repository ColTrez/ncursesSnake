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

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#ifndef __SNAKE_H__
#define __SNAKE_H__
#include "snake.h"
#endif

#define BOARD_WIDTH 80
#define BOARD_HEIGHT 40

int g_game_speed = 70;
int g_score = 0;

void game_loop(struct Queue* q, WINDOW* board);

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
	int scoreY = ((getmaxy(stdscr)/2)-(BOARD_HEIGHT/2))-1;
	int scoreX = ((getmaxx(stdscr)/2)-(BOARD_WIDTH/2))+1;
	mvaddstr(scoreY, scoreX, "Score: ");
	printw("%d", g_score);
	refresh();

	int height = getmaxy(stdscr);
	int width = getmaxx(stdscr);
	int startY = height/2;
	int startX = width/2;

	WINDOW* gameboard = newwin(BOARD_HEIGHT, BOARD_WIDTH, startY-(BOARD_HEIGHT/2), startX-(BOARD_WIDTH/2));
	box(gameboard, 0, 0);
	wrefresh(gameboard);

	Queue* snake = initial_snake(BOARD_HEIGHT/2, BOARD_HEIGHT/2);
	start_color();
	wattron(gameboard, COLOR_PAIR(1));
	//draw the initial snake
	Segment* sp = snake->head;
	while (sp->next != NULL){
		mvwaddch(gameboard, sp->y, sp->x, ACS_DIAMOND);
		sp = sp->next;
	}
	mvwaddch(gameboard, sp->y, sp->x, '>');

	wrefresh(gameboard);

	srand((unsigned int) time(NULL));//seed the random number generator for food placement
	game_loop(snake, gameboard);

	endwin();
	return 0;
}

void you_died(struct Queue* q, WINDOW* board, char headChar){
	init_pair(2, COLOR_RED, COLOR_BLACK);
	wattron(board, COLOR_PAIR(2));

	Segment* sp = q->head;
	while (sp->next != NULL){
		mvwaddch(board, sp->y, sp->x, ACS_DIAMOND);
		sp = sp->next;
	}
	mvwaddch(board, sp->y, sp->x, headChar);
	wrefresh(board);
	
	sleep(3);

	wattroff(board, COLOR_PAIR(2));
	int i;
	int j;
	for(i=1; i < getmaxx(board)-1; i++)
		for(j=1; j < getmaxy(board)-1; j++)
			mvwaddch(board, j, i, ' ');

	wattron(board, COLOR_PAIR(2));
	int messageX = getmaxx(board)/2;
	int messageY = getmaxy(board)/2;
	mvwaddstr(board, messageY, messageX-4, "YOU DIED");
	wrefresh(board);

	sleep(3);
}

void game_loop(struct Queue* q, WINDOW* board){
	int ch;
	bool foodPlaced = false;
	int screenWidth = getmaxx(board);
	int screenHeight = getmaxy(board);

	//color for the food
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
	
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
			//place a piece of food at a random location
			int foodX = (rand() % (screenWidth-1)) + 1;
			int foodY = (rand() % (screenHeight-1)) + 1;
			//check to make sure the location does overlap with the snake
			char atFoodPos = mvwinch(board, foodY, foodX) & A_CHARTEXT;
			if (atFoodPos == ' '){
				wattron(board, COLOR_PAIR(3));
				mvwaddch(board, foodY, foodX, '%');
				foodPlaced = true;
				wattron(board, COLOR_PAIR(1));
			}
		}
		
		//get new location of snake head
		Segment* newSnakeHead = create_segment(q->tail->y + moveY, q->tail->x + moveX);
		//replace old head char with body char
		mvwaddch(board, q->tail->y, q->tail->x, ACS_DIAMOND);
		
		//check new head for collision
		//first check for out of bounds
		if (newSnakeHead->x == 0 || newSnakeHead->x == screenWidth-1 || newSnakeHead->y == 0 || newSnakeHead->y == screenHeight-1){
			//then die
			moveX = 0;
			moveY = 0;
			you_died(q, board, headChar);
			return;
		}
		//then check if snake hit its own body or got some food
		char atPos = mvwinch(board, newSnakeHead->y, newSnakeHead->x) & A_CHARTEXT;
		if (atPos == (char) ACS_DIAMOND){
			//then die
			moveX = 0;
			moveY = 0;
			you_died(q, board, headChar);
			break;
		}
		else if (atPos == '%'){
			//snek eat food
			enqueue(q, newSnakeHead);
			mvwaddch(board, newSnakeHead->y, newSnakeHead->x, headChar);
			foodPlaced = false;
			//update score
			g_score++;
			int scoreY = ((getmaxy(stdscr)/2)-(BOARD_HEIGHT/2))-1;
			int scoreX = ((getmaxx(stdscr)/2)-(BOARD_WIDTH/2))+1;
			mvaddstr(scoreY, scoreX, "Score: ");
			printw("%d", g_score);
			refresh();
		}
		else {
			enqueue(q, newSnakeHead);
			mvwaddch(board, newSnakeHead->y, newSnakeHead->x, headChar);
			//replace tail with space
			Segment* oldSnakeTail = dequeue(q);
			mvwaddch(board, oldSnakeTail->y, oldSnakeTail->x, ' ');
			free(oldSnakeTail);
		}

		//refresh();
		wrefresh(board);
	}

}
