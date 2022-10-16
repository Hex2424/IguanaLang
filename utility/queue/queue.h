
/**
 * @file queue.h
 *
 * @copyright This file is a part of the project hivelang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author jorisb
 *
 * @date 2022-09-26
 */

#ifndef UTILITY_QUEUE_QUEUE_H_
#define UTILITY_QUEUE_QUEUE_H_
#include "stdint.h"
#include "stdbool.h"

typedef struct QNode QNode_t;
typedef QNode_t* QNodeHandle_t;

typedef struct QNode
{
	void* data;
	QNodeHandle_t next;
}QNode_t;

typedef struct
{
	QNodeHandle_t front;
	QNodeHandle_t back;
	uint64_t count;
}Queue_t;

typedef Queue_t* QueueHandle_t;

bool Queue_create(QueueHandle_t queue);
void Queue_destroy(QueueHandle_t queue);
void Queue_enqueue(QueueHandle_t queue, void* data);
void* Queue_dequeue(QueueHandle_t queue);
void* Queue_peek(QueueHandle_t queue);

#endif // UTILITY_QUEUE_QUEUE_H_