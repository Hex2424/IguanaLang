#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "dstack.h"

#define INITIAL_CAPACITY 8

bool Stack_create(DynamicStack_t *stack) {
    stack->data = malloc(sizeof(void*) * INITIAL_CAPACITY);
    if (!stack->data) return false;

    stack->size = 0;
    stack->capacity = INITIAL_CAPACITY;
    return true;
}

void Stack_destroy(DynamicStack_t *stack) {
    free(stack->data);
    stack->data = NULL;
    stack->size = 0;
    stack->capacity = 0;
}

bool Stack_push(DynamicStack_t *stack, void *item) {
    if (stack->size >= stack->capacity) {
        size_t newCapacity = stack->capacity * 2;
        void **newData = realloc(stack->data, sizeof(void*) * newCapacity);
        if (!newData) return false;
        stack->data = newData;
        stack->capacity = newCapacity;
    }
    stack->data[stack->size++] = item;
    return true;
}

void *Stack_pop(DynamicStack_t *stack) {
    if (stack->size == 0) return NULL;
    return stack->data[--stack->size];
}

void *Stack_peek(DynamicStack_t *stack) {
    if (stack->size == 0) return NULL;
    return stack->data[stack->size - 1];
}

bool Stack_isEmpty(DynamicStack_t *stack) {
    return stack->size == 0;
}