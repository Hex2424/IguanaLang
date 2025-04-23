#ifndef DYNAMIC_STACK_H
#define DYNAMIC_STACK_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void **data;       // Array of void* (generic)
    size_t size;       // Number of elements currently in stack
    size_t capacity;   // Allocated size
} DynamicStack_t;

/**
 * @brief Initializes the dynamic stack.
 * 
 * @param stack Pointer to DynamicStack_t structure
 * @return true on success, false on failure (e.g., allocation)
 */
bool Stack_create(DynamicStack_t *stack);

/**
 * @brief Destroys the stack and frees all associated memory.
 * 
 * @param stack Pointer to DynamicStack_t structure
 */
void Stack_destroy(DynamicStack_t *stack);

/**
 * @brief Pushes an element onto the stack.
 * 
 * @param stack Pointer to DynamicStack_t structure
 * @param item Pointer to item to push (can be any pointer type)
 * @return true on success, false on allocation failure
 */
bool Stack_push(DynamicStack_t *stack, void *item);

/**
 * @brief Pops the top element from the stack.
 * 
 * @param stack Pointer to DynamicStack_t structure
 * @return Pointer to the top item, or NULL if empty
 */
void *Stack_pop(DynamicStack_t *stack);

/**
 * @brief Peeks the top element without removing it.
 * 
 * @param stack Pointer to DynamicStack_t structure
 * @return Pointer to top item, or NULL if empty
 */
void *Stack_peek(DynamicStack_t *stack);

/**
 * @brief Checks if the stack is empty.
 * 
 * @param stack Pointer to DynamicStack_t structure
 * @return true if empty, false otherwise
 */
bool Stack_isEmpty(DynamicStack_t *stack);

#ifdef __cplusplus
}
#endif

#endif // DYNAMIC_STACK_H
