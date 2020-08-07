struct Queue {
	int* array;
	int front;
	int rear;
	int count;
	int capacity;
};

struct Queue* create_queue(int capacity);
int queue_peek(struct Queue* queue);
int queue_isEmpty(struct Queue* queue);
int queue_isFull(struct Queue* queue);
int size(struct Queue* queue);
void enqueue(struct Queue* queue, int item);
int dequeue(struct Queue* queue);
