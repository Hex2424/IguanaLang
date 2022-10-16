
/**
 * @file queue.c
 *
 * @copyright This file is a part of the project hivelang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author jorisb
 *
 * @date 2022-09-26
 */

#include <queue.h>

#include <assert.h>
#include <stdlib.h>

struct Queue Queue_create(
	void)
{
	struct Queue queue = {0};
	queue.front = NULL;
	queue.back = NULL;
	return queue;
}

void Queue_destroy(
	struct Queue* const queue)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The queue, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(queue != NULL);

	for (struct QNode* iterator = queue->front; iterator != NULL;)
	{
		struct QNode* current = iterator;

		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The current node, must never ever be null. The iterator, having
		//        value of null, should necer enter the loop.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(current != NULL);

		iterator = iterator->next;
		free(current);
	}

	queue->count = 0;
}

void Queue_enqueue(
	struct Queue* const queue,
	void* data)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The queue, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(queue != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The data, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(data != NULL);

	if (queue->front == NULL)
	{
		queue->back = (struct QNode*)malloc(sizeof(struct QNode));

		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The memory allocation errors can happen anytime, no matter build
		//        configuration being debug or release. However, since the compiler
		//        cannot prevent such bugs, I will leave it as assert. Worst case
		//        scenario - the compiler crashes, and user re-runs it.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(queue->back != NULL);

		struct QNode* node = queue->back;
		node->data = data;
		node->next = NULL;
		queue->front = queue->back;
	}
	else
	{
		struct QNode* node = (struct QNode*)malloc(sizeof(struct QNode));

		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The memory allocation errors can happen anytime, no matter build
		//        configuration being debug or release. However, since the compiler
		//        cannot prevent such bugs, I will leave it as assert. Worst case
		//        scenario - the compiler crashes, and user re-runs it.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(node != NULL);

		node->data = data;
		node->next = NULL;
		queue->back->next = node;
		queue->back = node;
	}

	++queue->count;
}

void* Queue_dequeue(
	struct Queue* const queue)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The queue, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(queue != NULL);

	if (queue->front == NULL)
	{
		return NULL;
	}

	struct QNode* node = queue->front;
	queue->front = node->next;
	void* data = node->data;
	free(node);
	--queue->count;
	return data;
}

void* Queue_peek(
	struct Queue* const queue)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The queue, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(queue != NULL);

	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. An element should never be dequeued from an empty queue.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(queue->front != NULL);

	struct QNode* node = queue->front;
	void* data = node->data;
	return data;
}
