#ifndef UTILITY_STACK_STACK_H_
#define UTILITY_STACK_STACK_H_

#include <stddef.h>
#include <stdbool.h>

#define STACK_MAX_SIZE 256  // Change this based on expected usage

typedef struct {
	void* data[STACK_MAX_SIZE];
	size_t top;
} Stack_t, *StackHandle_t;

bool Stack_create(StackHandle_t stack);
void Stack_destroy(StackHandle_t stack);
bool Stack_push(StackHandle_t stack, void* value);
void* Stack_pop(StackHandle_t stack);
void* Stack_peek(StackHandle_t stack);
bool Stack_isFull(const StackHandle_t stack);
bool Stack_isEmpty(const StackHandle_t stack);

#endif // UTILITY_STACK_STACK_H_
