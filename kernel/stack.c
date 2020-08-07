#include <stddef.h>
#include "stack.h"

struct Stack* create_stack(unsigned capacity) {
	struct Stack* stack;
	stack->top = -1;
	stack->capacity = capacity;
	stack->array;
	return stack;
}

int stack_isFull(struct Stack* stack) {
	return stack->top == stack->capacity - 1;
}

int stack_isEmpty(struct Stack* stack) {
	return stack->top == -1;
}

void push(struct Stack* stack, int item) {
	if(stack_isFull(stack))
		return;
	stack->array[++stack->top] = item;
}

int pop(struct Stack* stack) {
	if(stack_isEmpty(stack))
		return NULL;
	return stack->array[stack->top--];
}

int stack_peek(struct Stack* stack) {
	if(stack_isEmpty(stack))
		return NULL;
	return stack->array[stack->top];
}
