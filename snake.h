#ifndef __STDLIB_H__
#define __STDLIB_H__
#include <stdlib.h>
#endif
//segment of the snake's body, implemented as a singly linked list
struct Segment{
	int y;
	int x;
	struct Segment* next;
};
typedef struct Segment Segment;

//the snake is implemented as a queue of segments
struct Queue{
	struct Segment* head;
	struct Segment* tail;
	//counter-intuitively, the head of the snake is the tail of the queue
};
typedef struct Queue Queue;

struct Segment* create_segment(int newY, int newX);

void enqueue(struct Queue* q, struct Segment* s);

struct Segment* dequeue(struct Queue* q);

struct Queue* initial_snake(int snakeHeadY, int snakeHeadX);
