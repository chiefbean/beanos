struct Stack {
	int top;
	unsigned capacity;
	int* array;
};

struct Stack* create_stack(unsigned capacity);
int stack_isFull(struct Stack* stack);
int stack_isEmpty(struct Stack* stack);
void push(struct Stack* stack, int item);
int pop(struct Stack* stack);
int stack_peek(struct Stack* stack);
