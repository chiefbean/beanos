#include "queue.h"

struct Queue* create_queue(int capacity) {
	struct Queue* queue;
	queue->front = 0;
	queue->rear = -1;
	queue->count = 0;
	queue->capacity = capacity;
	queue->array;
	return queue;
}

int queue_peek(struct Queue* queue) {
	return queue->array[queue->front];
}

int queue_isEmpty(struct Queue* queue) {
	return queue->count == 0;
}

int queue_isFull(struct Queue* queue) {
	return queue->count == queue->capacity;
}

int size(struct Queue* queue) {
	return queue->capacity;
}

void enqueue(struct Queue* queue, int item) {
	if(!queue_isFull(queue)) {
		if(queue->rear == queue->capacity - 1) {
			queue->rear = -1;
		}
		queue->array[++queue->rear] = item;
		queue->count++;
	}
}

int dequeue(struct Queue* queue) {
	int data = queue->array[queue->front];
	
	if(queue->front == queue->capacity) {
		queue->front = 0;
	}
	queue->count--;
	return data;
}
