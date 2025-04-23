#include "stack.h"
#include <assert.h>

bool Stack_create(StackHandle_t stack)
{
	if (stack == NULL)
	{
		return false;
	}

	stack->top = 0;
	return true;
}

void Stack_destroy(StackHandle_t stack)
{
	assert(stack != NULL);

	// Zeroing out memory isn't strictly necessary, but can help with debug
	for (size_t i = 0; i < stack->top; ++i)
	{
		stack->data[i] = NULL;
	}
	stack->top = 0;
}

bool Stack_push(StackHandle_t stack, void* value)
{
	assert(stack != NULL);
	assert(value != NULL);

	if (stack->top >= STACK_MAX_SIZE)
	{
		return false; // Stack overflow
	}

	stack->data[stack->top++] = value;
	return true;
}

void* Stack_pop(StackHandle_t stack)
{
	assert(stack != NULL);

	if (stack->top == 0)
	{
		return NULL; // Stack underflow
	}

	void* value = stack->data[--stack->top];
	stack->data[stack->top] = NULL;
	return value;
}

void* Stack_peek(StackHandle_t stack)
{
	assert(stack != NULL);
	assert(stack->top > 0);

	return stack->data[stack->top - 1];
}

bool Stack_isFull(const StackHandle_t stack)
{
	assert(stack != NULL);
	return stack->top == STACK_MAX_SIZE;
}

bool Stack_isEmpty(const StackHandle_t stack)
{
	assert(stack != NULL);
	return stack->top == 0;
}
