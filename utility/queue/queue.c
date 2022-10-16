
/**
 * @file queue.c
 *
 * @copyright This file is a part of the project IguanaLang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author jorisb
 *
 * @date 2022-09-26
 */

#include "queue.h"
#include <assert.h>
#include <stdlib.h>




bool Queue_create(QueueHandle_t queue)
{
	if(queue == NULL)
	{
		return false;
	}

	queue->front = NULL;
	queue->back = NULL;
	return true;
}

void Queue_destroy(QueueHandle_t queue)
{
	// NOTE: using `assert` and not `if`
	// REASONS:
	//     1. The queue, provided to this function, must never ever be null.
	//     2. This assert will prevent developers infliced bugs and development
	//        and debug configuration.
	assert(queue != NULL);

	for (QNodeHandle_t iterator = queue->front; iterator != NULL;)
	{
		QNodeHandle_t current = iterator;

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

void Queue_enqueue(QueueHandle_t queue, void* data)
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
		queue->back = (QNodeHandle_t)malloc(sizeof(QNode_t));

		// NOTE: using `assert` and not `if`
		// REASONS:
		//     1. The memory allocation errors can happen anytime, no matter build
		//        configuration being debug or release. However, since the compiler
		//        cannot prevent such bugs, I will leave it as assert. Worst case
		//        scenario - the compiler crashes, and user re-runs it.
		//     2. This assert will prevent developers infliced bugs and development
		//        and debug configuration.
		assert(queue->back != NULL);

		QNodeHandle_t node = queue->back;
		node->data = data;
		node->next = NULL;
		queue->front = queue->back;
	}
	else
	{
		QNodeHandle_t node = (QNodeHandle_t)malloc(sizeof(QNode_t));

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

void* Queue_dequeue(QueueHandle_t queue)
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

	QNodeHandle_t node = queue->front;
	queue->front = node->next;
	void* data = node->data;
	free(node);
	--queue->count;
	return data;
}

void* Queue_peek(QueueHandle_t queue)
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

	QNodeHandle_t node = queue->front;
	void* data = node->data;
	return data;
}
