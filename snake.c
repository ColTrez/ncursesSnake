#ifndef __SNAKE_H__
#include "snake.h"
#endif

#ifndef __STDLIB_H__
#include <stdlib.h>
#endif

struct Segment* create_segment(int newY, int newX){
	Segment* newSegment = malloc(sizeof(Segment));
	newSegment->y = newY;
	newSegment->x = newX;
	newSegment->next = NULL;

	return newSegment;
}

void enqueue(struct Queue* q, struct Segment* s){
	q->tail->next = s;
	q->tail = s;
}

//this implementation does not free the memory of the old head, that is the responsibility of the caller
struct Segment* dequeue(struct Queue* q){
	Segment* oldHead = q->head;
	q->head = oldHead->next;
	return oldHead;
}

//new snake will have head at (snakeHeadX, snakeHeadY) with body trailing behind 2 X positions
//caller needs to be careful that snakeHeadX is at least 3 spaces from the edge of the window
struct Queue* initial_snake(int snakeHeadY, int snakeHeadX){
	Segment* head = create_segment(snakeHeadY, snakeHeadX);
	Segment* body = create_segment(snakeHeadY, snakeHeadX-1);
	Segment* tail = create_segment(snakeHeadY, snakeHeadX-2);
	
	tail->next = body;
	body->next = head;
	//heads next is left as null

	Queue* snake = malloc(sizeof(Queue));
	//in order to facilitate movement of the snake,
	//the head of the snake is the tail of the queue
	//and the tail of the snake is the head of the queue
	snake->head = tail;
	snake->tail = head;

	return snake;
}

